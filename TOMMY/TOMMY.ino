//=======================================================================
//  Menu principal- No modificar
//   by: Aniel AF
// ====================================================================
#include <Wire.h>
#include "hexapodo_config.h"
#include "pca9685.h"
#include "cinematica.h"
#include "calibracion.h"
#include "web_server.h"

void setup() {
  Serial.begin(115200);
  delay(500);
  Serial.println();
  Serial.println("===== HEXAPOD BOOT =====");

  // 1) Bus I2C
  Wire.begin(I2C_SDA_PIN, I2C_SCL_PIN, I2C_FREQ_HZ);
  Serial.printf("I2C iniciado: SDA=%d SCL=%d freq=%d\n",
                I2C_SDA_PIN, I2C_SCL_PIN, I2C_FREQ_HZ);

  // 2) Inicializar PCA9685(s)
  pca9685_init(PCA9685_ADDR_1, PCA9685_PWM_FREQ);
  pca9685_init(PCA9685_ADDR_2, PCA9685_PWM_FREQ);  // si no estaconectado o esta mal conectado
  // fallara, pa que me entiendan se van a replicar los movimientos

  // 3) Cargar calibracion (de Preferences si existe, sino defaults)
  cal_init();

  // 4) NO mover los servos al arranque hasta que el usuario los calibre.
  //    Una vez calibrado, puedes descomentar pose_sit() abajo.
  // pose_sit();

  // 5) WiFi + servidor web
  web_server_start();

  Serial.println("Listo. Conectate a WiFi 'Hexapod' y abre http://192.168.4.1");
}

void loop() {
  web_server_handle();   // procesar peticiones HTTP
  delay(2);
}
