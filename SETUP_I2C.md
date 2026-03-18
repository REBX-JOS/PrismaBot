# Setup Comunicación I2C entre ESP32-CAM y NodeMCU

## 🏗️ Arquitectura Final

```
┌─────────────────┐               ┌──────────────────┐
│  ESP32-CAM      │               │  NodeMCU ESP32   │
│  (MASTER I2C)   │               │  (SLAVE I2C)     │
│                 │               │                  │
│ GPIO 14 (SDA) ──┼───────────────┼── GPIO 21 (SDA) │
│ GPIO 15 (SCL) ──┼───────────────┼── GPIO 22 (SCL) │
│ GND           ──┼───────────────┼── GND           │
└─────────────────┘               │                  │
  Detecta Colores                 │ LCD I2C 0x27     │
  Manda por I2C                   │ (GPIO 2/4)       │
                                  │                  │
                                  │ Motors (GPIO14-17)
                                  └──────────────────┘
```

## ✅ Configuración de Pines

### ESP32-CAM (Maestro I2C)
| Función | GPIO | Uso |
|---------|------|-----|
| SDA | 14 | I2C a NodeMCU |
| SCL | 15 | I2C a NodeMCU |
| GND | GND | Referencia común |

### NodeMCU ESP32 (Esclavo I2C)
| Función | GPIO | Uso |
|---------|------|-----|
| LCD SDA | 2 | I2C LCD (0x27) |
| LCD SCL | 4 | I2C LCD (0x27) |
| CAM SDA | 21 | I2C secundario (0x08) |
| CAM SCL | 22 | I2C secundario (0x08) |
| Motor A IN1 | 14 | Motor izquierdo |
| Motor A IN2 | 15 | Motor izquierdo |
| Motor B IN3 | 12 | Motor derecho |
| Motor B IN4 | 13 | Motor derecho |

## 🔌 Conexión Física

```
Cables necesarios:
- 1x SDA (GPIO14 ESP32-CAM → GPIO21 NodeMCU)
- 1x SCL (GPIO15 ESP32-CAM → GPIO22 NodeMCU)
- 1x GND (ESP32-CAM → NodeMCU)

Resistencias Pull-up:
- 2x 4.7kΩ entre SDA/SCL y 3.3V (si la LCD no las tiene)
```

## 🧪 Plan de Prueba

### PASO 1: Verificar LCD (NodeMCU)
1. Solo flashear NodeMCU
2. Debe mostrar en pantalla: "PrismaBot v1.0" y "Esperando colores..."
3. Ver en Serial Monitor (115200 baud): `[LCD] Ready`

### PASO 2: Detectar Colores (ESP32-CAM)
1. Conectar cámara a buena iluminación
2. Flashear ESP32-CAM
3. Ver en Serial Monitor: `[CAM] Detected: 1-7` (números del color)
4. Variar colores frente a la cámara para verificar detección

### PASO 3: Prueba I2C (Ambos juntos)
1. Conectar física mente los cables I2C (GPIO14/15 ESP32-CAM a GPIO21/22 NodeMCU)
2. Flashear ambos
3. En Serial Monitor del ESP32-CAM debe ver:
   - `[I2C] NodeMCU found at 0x08`
   - `[I2C] Sent color X to NodeMCU`
   
4. En Serial Monitor del NodeMCU debe ver:
   - `[I2C_CAM] Slave mode enabled at 0x08`
   - `[I2C] Received color: X`
   - LCD mostrará color y acción
   - Motores ejecutarán

### PASO 4: Prueba End-to-End
1. Colocar objeto de color diferente frente a cámara
2. Observar:
   - Serial ESP32-CAM: Color detectado
   - I2C: Envío por I2C
   - Serial NodeMCU: Recepción y acción
   - LCD: Muestra color y acción
   - Motores: Ejecutan movimiento

## 📋 Mapeo de Colores → Acciones

| Color | ID | Acción |
|-------|----|----|
| NONE | 0 | STOP |
| RED (1-10, 170-180 HSV) | 1 | TURN_RIGHT |
| GREEN (40-80 HSV) | 2 | STOP |
| YELLOW (20-35 HSV) | 3 | BACKWARD |
| BLUE (100-140 HSV) | 4 | TURN_180 |
| WHITE (0-180 H, 0-50 S, 200-255 V) | 5 | ADVANCE |
| PINK (140-170 HSV) | 6 | BACKWARD |
| BLACK (0-180 H, 0-255 S, 0-50 V) | 7 | ADVANCE |

## 🔧 Ajustes y Debugging

### Si I2C no funciona:
1. Verificar pines de conexión
2. Ver Serial Monitor del ESP32-CAM: ¿Dice `[I2C] NodeMCU found`?
3. Si no lo encuentra, verificar dirección I2C del NodeMCU
4. Probar con I2C Scanner

### Si colores no se detectan:
1. Verificar iluminación
2. Aumentar `COLOR_DETECT_THRESHOLD` en color_config.h
3. Ajustar rangos HSV en color_config.h

### Si los motores no se mueven:
1. Verificar cables GPIO
2. Ver si el LED de motor se enciende
3. Revisar voltaje en motor

## ✨ Notas Importantes

- **Baud Rate**: Serial = 115200 (ambos ESP32)
- **Frecuencia I2C**: 100kHz
- **Voltaje**: 3.3V (ambos chips)
- **Timeout detección**: 200ms entre cámaras, 50ms entre lecturas motor
- **LCD**: Muestra estado en tiempo real

## 🎯 Próximos Pasos

1. Calibrar rangos HSV según tus colores reales
2. Ajustar tiempos de movimiento (MOTOR_*_MS)
3. Agregar feedback (LEDs, sonidos)
4. Crear casos de test en pantalla
