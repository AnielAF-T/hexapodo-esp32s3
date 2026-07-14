// ====================================================================
//   Esto es solo para obtener valores de posicion del servo 
//   Si todos los servos estan calibrados ignorar
//   by: Aniel AF
// ====================================================================

#ifndef CALIBRATION_H
#define CALIBRATION_H

#include <Arduino.h>
#include <Preferences.h>
#include "hexapodo_config.h"
#include "pca9685.h"

struct ServoCal {
  uint16_t pulse_min;
  uint16_t pulse_max;
  float    offset_deg;
  int8_t   direction;
};

static ServoCal g_servo_cal[NUM_SERVOS];

static inline int cal_idx(int leg, int joint) { return leg * DOF_PER_LEG + joint; }

static inline float cal_clampf(float v, float lo, float hi) {
  return v < lo ? lo : (v > hi ? hi : v);
}

// ---- ruteo: lee directamente SERVO_PCA_ADDR y SERVO_CHANNEL ----
//      ya no hay logica "si canal > 16"; cada servo dice donde va.
static inline void cal_route_pulse(int leg, int joint, uint16_t pulse) {
  uint8_t addr = SERVO_PCA_ADDR[leg][joint];
  uint8_t ch   = SERVO_CHANNEL[leg][joint];
  pca9685_set_pulse(addr, ch, pulse);
}

// ---- persistencia con Preferences (NVS) ----
static bool cal_save_nvs() {
  Preferences p;
  if (!p.begin("hexcal", false)) return false;
  size_t n = p.putBytes("cal", g_servo_cal, sizeof(g_servo_cal));
  p.end();
  Serial.printf("[CAL] guardado %u bytes\n", n);
  return n == sizeof(g_servo_cal);
}

static bool cal_load_nvs() {
  Preferences p;
  if (!p.begin("hexcal", true)) return false;
  size_t n = p.getBytesLength("cal");
  if (n != sizeof(g_servo_cal)) { p.end(); return false; }
  p.getBytes("cal", g_servo_cal, sizeof(g_servo_cal));
  p.end();
  return true;
}

// ---- init: defaults + NVS si hay datos ----
static inline void cal_init() {
  for (int leg = 0; leg < NUM_LEGS; ++leg) {
    for (int j = 0; j < DOF_PER_LEG; ++j) {
      int k = cal_idx(leg, j);
      g_servo_cal[k].pulse_min  = SERVO_PULSE_MIN;
      g_servo_cal[k].pulse_max  = SERVO_PULSE_MAX;
      g_servo_cal[k].offset_deg = SERVO_OFFSET_DEG_DEFAULT[leg][j];
      g_servo_cal[k].direction  = SERVO_DIRECTION_DEFAULT[leg][j];
    }
  }
  if (cal_load_nvs()) Serial.println("[CAL] calibracion cargada de NVS");
  else                Serial.println("[CAL] usando defaults");
}

static inline void cal_set_raw_pulse(int leg, int joint, uint16_t pulse) {
  if (leg < 0 || leg >= NUM_LEGS) return;
  if (joint < 0 || joint >= DOF_PER_LEG) return;
  if (pulse < 50) pulse = 50;
  if (pulse > 600) pulse = 600;
  cal_route_pulse(leg, joint, pulse);
}

static inline void cal_apply_logical_angle(int leg, int joint, float deg_logico) {
  int k = cal_idx(leg, joint);
  ServoCal *c = &g_servo_cal[k];
  float phys = c->direction * deg_logico + c->offset_deg;
  phys = cal_clampf(phys, SERVO_MIN_DEG, SERVO_MAX_DEG);
  float t = (phys - SERVO_MIN_DEG) / (SERVO_MAX_DEG - SERVO_MIN_DEG);
  uint16_t pulse = (uint16_t)(c->pulse_min + t * (c->pulse_max - c->pulse_min));
  cal_route_pulse(leg, joint, pulse);
}

static inline void cal_capture_offset(int leg, int joint, uint16_t pulse_actual, float deg_objetivo) {
  int k = cal_idx(leg, joint);
  ServoCal *c = &g_servo_cal[k];
  float t = (float)(pulse_actual - c->pulse_min) / (float)(c->pulse_max - c->pulse_min);
  float phys = SERVO_MIN_DEG + t * (SERVO_MAX_DEG - SERVO_MIN_DEG);
  c->offset_deg = phys - c->direction * deg_objetivo;
  Serial.printf("[CAL] L%d J%d pulse=%u -> phys=%.2f offset=%.2f\n",
                leg, joint, pulse_actual, phys, c->offset_deg);
}

static inline String cal_dump_as_c() {
  String s;
  s += "// --- pega esto en hexapod_config.h ---\n";
  s += "static const float SERVO_OFFSET_DEG_DEFAULT[NUM_LEGS][DOF_PER_LEG] = {\n";
  for (int leg = 0; leg < NUM_LEGS; ++leg) {
    char buf[80];
    snprintf(buf, sizeof(buf), "    {%7.2ff, %7.2ff, %7.2ff}%s\n",
             g_servo_cal[cal_idx(leg,0)].offset_deg,
             g_servo_cal[cal_idx(leg,1)].offset_deg,
             g_servo_cal[cal_idx(leg,2)].offset_deg,
             leg < NUM_LEGS-1 ? "," : "");
    s += buf;
  }
  s += "};\n\nstatic const int8_t SERVO_DIRECTION_DEFAULT[NUM_LEGS][DOF_PER_LEG] = {\n";
  for (int leg = 0; leg < NUM_LEGS; ++leg) {
    char buf[64];
    snprintf(buf, sizeof(buf), "    {%+d, %+d, %+d}%s\n",
             g_servo_cal[cal_idx(leg,0)].direction,
             g_servo_cal[cal_idx(leg,1)].direction,
             g_servo_cal[cal_idx(leg,2)].direction,
             leg < NUM_LEGS-1 ? "," : "");
    s += buf;
  }
  s += "};\n";
  return s;
}

#endif
