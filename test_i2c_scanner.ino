// ═════════════════════════════════════════════════════════════
// TEST I2C SCANNER - Para ambos ESP32
// Copia este código en un sketch vacío para verificar I2C
// ═════════════════════════════════════════════════════════════

#include <Wire.h>

// OPCIÓN 1: Para ESP32-CAM (Maestro)
#define I2C_SDA 14
#define I2C_SCL 15

// OPCIÓN 2: Para NodeMCU (Esclavo)
// #define I2C_SDA 21
// #define I2C_SCL 22

void setup() {
    Serial.begin(115200);
    delay(2000);
    Serial.println("\n\n=== I2C SCANNER ===");
    
    Wire.begin(I2C_SDA, I2C_SCL);
    Serial.println("I2C initialized");
    Serial.print("Scanning I2C devices on SDA="); Serial.print(I2C_SDA);
    Serial.print(", SCL="); Serial.println(I2C_SCL);
    Serial.println();
    
    scanI2C();
}

void scanI2C() {
    byte error, address;
    int nDevices = 0;
    
    for (address = 0x01; address < 0x7F; address++) {
        Wire.beginTransmission(address);
        error = Wire.endTransmission();
        
        if (error == 0) {
            Serial.print("I2C device found at address 0x");
            if (address < 16) Serial.print("0");
            Serial.print(address, HEX);
            Serial.println(" !");
            nDevices++;
            
            // Identifica dispositivos conocidos
            if (address == 0x27 || address == 0x3F) {
                Serial.println("  → Likely: LCD Display");
            } else if (address == 0x08) {
                Serial.println("  → Likely: ESP32 NodeMCU (Slave)");
            }
        } else if (error == 4) {
            Serial.print("Unknown error at address 0x");
            if (address < 16) Serial.print("0");
            Serial.println(address, HEX);
        }
    }
    
    if (nDevices == 0) {
        Serial.println("No I2C devices found\n");
    } else {
        Serial.print("Found ");
        Serial.print(nDevices);
        Serial.println(" device(s)\n");
    }
}

void loop() {
    delay(5000);
    Serial.println("--- Scanning again ---");
    scanI2C();
}
