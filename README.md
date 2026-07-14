# 🤖 Hexápodo ESP32-S3 (18 DOF)

<img width="1195" height="896" alt="Hexapodo" src="https://github.com/user-attachments/assets/6c94f13f-dd2c-4212-bc01-8afc711bfea2" />

## 📋 Descripción

Robot hexápodo (6 patas, 3 servos por pata = 18 grados de libertad) controlado por un ESP32-S3. Incluye cinemática inversa propia, control de motores mediante PCA9685 e interfaz gráfica para operarlo.

## ✨ Características

- 18 grados de libertad (3 por pata)
- Cinemática inversa calculada desde cero
- Control de hasta 18 servos vía módulo PCA9685 (PWM I2C)
- Interfaz gráfica para control manual/remoto
- [Agrega: modos de marcha (trípode, ondulante), autonomía, sensores, etc.]

## 🛠️ Tecnologías

`ESP32-S3` `C++ / Arduino Framework` `PCA9685` `HTML (Interfaz)` `solidwork`
## 📁 Estructura del repo

```
hexapodo-esp32s3/
├── control-motores/       # Control de servos vía PCA9685
├── cinematica-inversa/    # Cálculos de cinemática inversa
├── interfaz-grafica/      # Interfaz para control del robot
├── docs/                  # Fotos, gifs, diagramas
└── README.md
```

## 🚀 Cómo usarlo

```bash
# Ejemplo — ajusta según tu setup real
1. Clona el repo
2. Abre el proyecto en Arduino IDE / PlatformIO
3. Configura los pines y la dirección I2C del PCA9685
4. Sube el firmware al ESP32-S3
5. Ejecuta la interfaz gráfica para controlarlo
```

## 📸 Galería

<!-- Agrega fotos o gifs del robot caminando, la interfaz, el ensamble -->

## 🔧 Posibles mejoras futuras

- [ ] [Ej: agregar sensores de contacto en las patas]
- [ ] [Ej: navegación autónoma]

## 📄 Licencia

MIT
