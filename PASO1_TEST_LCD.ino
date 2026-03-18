// ═══════════════════════════════════════════════════════════
// PASO 1: TEST SOLO LCD - Sin I2C CAM
// Compila y flashea SOLO esto en el NodeMCU
// ═══════════════════════════════════════════════════════════

#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// GPIO I2C para LCD
#define I2C_SDA_LCD  2
#define I2C_SCL_LCD  4
#define DISPLAY_ADDR 0x27
#define LCD_COLS     20
#define LCD_ROWS     4

// LCD 20x4
LiquidCrystal_I2C lcd(DISPLAY_ADDR, LCD_COLS, LCD_ROWS);
bool lcd_ready = false;

void setup() {
    Serial.begin(115200);
    delay(1000);
    
    Serial.println("\n\n=== PASO 1: TEST LCD ONLY ===");
    Serial.print("Inicializando I2C en SDA=GPIO");
    Serial.print(I2C_SDA_LCD);
    Serial.print(", SCL=GPIO");
    Serial.println(I2C_SCL_LCD);
    
    // Inicializar I2C para LCD
    Wire.begin(I2C_SDA_LCD, I2C_SCL_LCD);
    delay(500);
    
    // Verificar si LCD está en bus I2C
    Wire.beginTransmission(DISPLAY_ADDR);
    byte error = Wire.endTransmission();
    
    if (error == 0) {
        Serial.printf("[LCD] Encontrado en dirección 0x%02X\n", DISPLAY_ADDR);
    } else {
        Serial.printf("[LCD] ERROR: No encontrado en 0x%02X (error: %d)\n", DISPLAY_ADDR, error);
        Serial.println("→ Verifica: GND, SDA (GPIO2), SCL (GPIO4)");
        while(1) {
            digitalWrite(LED_BUILTIN, HIGH);
            delay(200);
            digitalWrite(LED_BUILTIN, LOW);
            delay(200);
        }
    }
    
    // Intentar inicializar LCD
    lcd.init();
    lcd.backlight();
    lcd_ready = true;
    
    Serial.println("[LCD] ✓ Iniciado correctamente");
    
    // Mostrar en pantalla
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("=== PRUEBA LCD ===");
    
    lcd.setCursor(0, 1);
    lcd.print("PrismaBot v1.0");
    
    lcd.setCursor(0, 2);
    lcd.print("Esperando colores...");
    
    lcd.setCursor(0, 3);
    lcd.print("LCD OK!");
    
    Serial.println("[LCD] Ready - Ver pantalla LCD");
    Serial.println("\n✓ PASO 1 COMPLETADO: LCD funcionando");
}

void loop() {
    // Parpadear LED cada 1 segundo para confirmar que está vivo
    digitalWrite(LED_BUILTIN, HIGH);
    delay(500);
    digitalWrite(LED_BUILTIN, LOW);
    delay(500);
    
    // Cada 5 segundos imprimir estado
    static uint32_t last_print = 0;
    if (millis() - last_print > 5000) {
        last_print = millis();
        Serial.println("[LCD] STATUS: OK - Presiona Ctrl+A Ctrl+M para ir a PASO 2");
    }
}
