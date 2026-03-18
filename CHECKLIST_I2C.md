// ═════════════════════════════════════════════════════════════
// CHECKLIST PRÁCTICO - Implementación I2C Completa
// ═════════════════════════════════════════════════════════════

## ✅ CAMBIOS REALIZADOS

### 1️⃣ ESP32-CAM (src/main.cpp)
- ✓ Agregado Wire.h para I2C
- ✓ Configurado I2C Maestro en GPIO 14 (SDA), GPIO 15 (SCL)
- ✓ Función i2c_init() para inicializar y detectar NodeMCU
- ✓ Función send_color_to_nodemcu() para enviar colores
- ✓ Loop modificado para enviar datos por I2C en lugar de UART
- ✓ Debug messages para Serial Monitor

### 2️⃣ NodeMCU ESP32 (src/main.cpp)
- ✓ Configurado I2C 0 para LCD (GPIO 2/4) - ya existía
- ✓ Configurado I2C 1 (secundario) como ESCLAVO en GPIO 21/22
- ✓ Callback i2c_onReceive() para recibir colores del ESP32-CAM
- ✓ Loop modificado para procesar colores recibidos por I2C
- ✓ LCD sigue funcionando normalmente

### 3️⃣ color_config.h (ESP32-CAM)
- ✓ Agregado enum Action con todas las acciones
- ✓ Agregado ACTION_NAMES[] para nombres legibles
- ✓ Agregado COLOR_NAMES[] para nombres de colores
- ✓ Mapeo completo de colores → acciones

### 4️⃣ robot_config.h (NodeMCU)
- ✓ Actualizado comentario: de UART a I2C
- ✓ Agregado enum Action
- ✓ Agregado ACTION_NAMES[]

## 🔌 CONEXIONES FÍSICAS REQUERIDAS

```
ESP32-CAM                    NodeMCU ESP32
GPIO 14 (SDA) ──────────────── GPIO 21 (SDA)
GPIO 15 (SCL) ──────────────── GPIO 22 (SCL)
GND ──────────────────────────── GND
```

**Importante**: 
- Usa cables cortos (< 30cm) para I2C
- Verifica que ambas tablas estén en el mismo nivel de tensión (3.3V)
- Las resistencias pull-up se pueden necesitar si la LCD no las proporciona

## 🎯 VERIFICACIÓN DE CONECTIVIDAD

### Paso 1: Test de LCD (Aislar)
```cpp
// En NodeMCU, comentar i2c_cam_init() en setup()
// Debe mostrar:
// - Serial: "[LCD] Ready"
// - Pantalla: "PrismaBot v1.0"
```

### Paso 2: Test de Cámara (Aislar)
```cpp
// En ESP32-CAM, sin NodeMCU conectado
// Debe mostrar:
// - Serial: "Color: 1-7" (valores numéricos)
// - Sin error I2C (esperado)
```

### Paso 3: Test I2C Scanner (Recomendado)
```cpp
// Usar archivo: test_i2c_scanner.ino
// Para ESP32-CAM: Ejecutar con GPIO 14/15
// Debe mostrar: "I2C device found at address 0x08" (NodeMCU)

// Para NodeMCU: Ejecutar con GPIO 21/22
// Si está en modo Slave puro, puede no verse en scanner
```

### Paso 4: Test Comunicación Full
```cpp
// Ambos dispositivos conectados por I2C
// Serial ESP32-CAM debe mostrar:
//   [I2C] NodeMCU found at 0x08
//   [I2C] Sent color X to NodeMCU

// Serial NodeMCU debe mostrar:
//   [I2C_CAM] Slave mode enabled at 0x08
//   [I2C] Received color: X
```

## 📊 TABLA DE VELOCIDADES

| Componente | Velocidad | Puerto |
|-----------|----------|---------|
| Serial Debug | 115200 baud | UART0 |
| I2C (LCD) | 100kHz | GPIO 2/4 |
| I2C (CAM) | 100kHz | GPIO 21/22 |

## 🚨 TROUBLESHOOTING

| Problema | Causa | Solución |
|----------|--------|----------|
| No se ve `[I2C] NodeMCU found` | NodeMCU no enciende o I2C error | Verificar conexión de pines, alimentación |
| LCD no muestra nada | Wire.begin() USA GPIO 2/4 por defecto | Ver si lcd_init() se ejecuta |
| No se reciben colores | I2C fail o dirección incorrecta | Usar I2C Scanner, verificar 0x08 |
| Serial Monitor basura | Baud rate incorrecto | Verificar 115200 en ambos IDEs |
| Motores no se mueven | GPIO no inicializado | Verificar pinMode() en setup() |

## 💾 ARCHIVOS A COMPILAR

### Para ESP32-CAM:
```
- src/main.cpp ✓ (MODIFICADO)
- include/camera_pins.h (sin cambios)
- include/color_config.h ✓ (MODIFICADO - agregados Actions)
```

### Para NodeMCU:
```
- src/main.cpp ✓ (MODIFICADO - I2C Slave)
- include/robot_config.h ✓ (MODIFICADO - agregados Actions)
- include/color_config.h (copiar desde ESP32-CAM)
```

## 📱 SALIDA ESPERADA EN SERIAL MONITOR

### ESP32-CAM (115200 baud):
```
=== ESP32-CAM COLOR DETECTOR ===
[CAM] Ready
[I2C] Ready. Scanning for NodeMCU...
[I2C] NodeMCU found at 0x08
Ready. Detecting colors via I2C...
[CAM] Detected: 1
[I2C] Sent color 1 to NodeMCU
[CAM] Detected: 3
[I2C] Sent color 3 to NodeMCU
```

### NodeMCU ESP32 (115200 baud):
```
=== ESP32 ROBOT CONTROLLER (I2C) ===
[LCD] Ready
[I2C_CAM] Slave mode enabled at 0x08 on GPIO 21/22
Waiting for color data from ESP32-CAM on I2C...
[I2C] Received color: 1
Processing: COLOR_RED (1)
Action: TURN_RIGHT
→ TURN_RIGHT
[I2C] Received color: 3
Processing: COLOR_YELLOW (3)
Action: BACKWARD
← BACKWARD
```

## ✨ VENTAJAS DE ESTA ARQUITECTURA I2C

✅ Comunicación bidireccional (posible agregar feedback)
✅ Misma velocidad de transferencia
✅ Usa menos pines que UART
✅ Compatible con LCD en el mismo bus
✅ Escalable (puedes agregar otro I2C slave si necesitas)
✅ Mejor sincronización maestro-esclavo

## 🔄 PRÓXIMOS PASOS OPCIONALES

1. Agregar comunicación inversa (NodeMCU → ESP32-CAM) para telemetría
2. Implementar handshaking para asegurar entrega
3. Agregar más modos de operación
4. Integrar sensor ultrasónico/infrarrojo
5. Crear protocolo I2C más robusto con checksums
