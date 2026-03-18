#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "robot_config.h"
#include "color_config.h"

// TWO separate I2C buses:
// Bus 0: LCD on GPIO 2/4
// Bus 1: ESP32-CAM on GPIO 21/22
#define I2C_SDA_LCD  2
#define I2C_SCL_LCD  4
#define I2C_SDA_CAM  21
#define I2C_SCL_CAM  22
#define CAM_I2C_ADDR  0x08
#define DISPLAY_ADDR_ALT 0x3F

// LCD 20x4 (on Wire/I2C 0)
LiquidCrystal_I2C lcd_27(DISPLAY_ADDR, LCD_COLS, LCD_ROWS);
LiquidCrystal_I2C lcd_3f(DISPLAY_ADDR_ALT, LCD_COLS, LCD_ROWS);
LiquidCrystal_I2C* lcd = nullptr;
bool lcd_ready = false;

// Second Wire object for ESP32-CAM (on GPIO 21/22)
TwoWire WireCAM = TwoWire(1);

uint8_t last_received_color = (uint8_t)COLOR_NONE;
uint8_t last_processed_color = (uint8_t)COLOR_NONE;

// ── Motor control ────────────────────────────────────────────

void motors_forward() {
    digitalWrite(MOTOR_A_IN1, HIGH);
    digitalWrite(MOTOR_A_IN2, LOW);
    digitalWrite(MOTOR_B_IN3, HIGH);
    digitalWrite(MOTOR_B_IN4, LOW);
}

void motors_backward() {
    digitalWrite(MOTOR_A_IN1, LOW);
    digitalWrite(MOTOR_A_IN2, HIGH);
    digitalWrite(MOTOR_B_IN3, LOW);
    digitalWrite(MOTOR_B_IN4, HIGH);
}

void motors_turn_right() {
    digitalWrite(MOTOR_A_IN1, HIGH);
    digitalWrite(MOTOR_A_IN2, LOW);
    digitalWrite(MOTOR_B_IN3, LOW);
    digitalWrite(MOTOR_B_IN4, HIGH);
}

void motors_turn_left() {
    digitalWrite(MOTOR_A_IN1, LOW);
    digitalWrite(MOTOR_A_IN2, HIGH);
    digitalWrite(MOTOR_B_IN3, HIGH);
    digitalWrite(MOTOR_B_IN4, LOW);
}

void motors_stop() {
    digitalWrite(MOTOR_A_IN1, LOW);
    digitalWrite(MOTOR_A_IN2, LOW);
    digitalWrite(MOTOR_B_IN3, LOW);
    digitalWrite(MOTOR_B_IN4, LOW);
}

// ── Execute action ──────────────────────────────────────────

void execute_action(Action action) {
    switch (action) {
        case ACT_ADVANCE:
            Serial.println("→ ADVANCE");
            motors_forward();
            delay(MOTOR_ADVANCE_MS);
            motors_stop();
            break;

        case ACT_BACKWARD:
            Serial.println("← BACKWARD");
            motors_backward();
            delay(MOTOR_ADVANCE_MS);
            motors_stop();
            break;

        case ACT_TURN_RIGHT:
            Serial.println("↻ TURN_RIGHT");
            motors_turn_right();
            delay(MOTOR_TURN_90_MS);
            motors_stop();
            break;

        case ACT_TURN_LEFT:
            Serial.println("↺ TURN_LEFT");
            motors_turn_left();
            delay(MOTOR_TURN_90_MS);
            motors_stop();
            break;

        case ACT_TURN_180:
            Serial.println("⟳ TURN_180");
            motors_turn_right();
            delay(MOTOR_TURN_180_MS);
            motors_stop();
            break;

        case ACT_STOP:
        default:
            Serial.println("⊗ STOP");
            motors_stop();
            break;
    }
}

// ── LCD display ──────────────────────────────────────────────

void i2c_scan_bus() {
    Serial.printf("[I2C] Bus 0 (LCD) - Scanning SDA=%d SCL=%d...\n", I2C_SDA_LCD, I2C_SCL_LCD);
    int found = 0;
    for (uint8_t addr = 1; addr < 0x7F; addr++) {
        Wire.beginTransmission(addr);
        uint8_t err = Wire.endTransmission();
        if (err == 0) {
            Serial.printf("[I2C] Device found at 0x%02X\n", addr);
            found++;
        }
    }
    if (found == 0) {
        Serial.println("[I2C] No devices found on Bus 0");
    }
    
    Serial.printf("[I2C] Bus 1 (CAM) - Scanning SDA=%d SCL=%d...\n", I2C_SDA_CAM, I2C_SCL_CAM);
    found = 0;
    for (uint8_t addr = 1; addr < 0x7F; addr++) {
        WireCAM.beginTransmission(addr);
        uint8_t err = WireCAM.endTransmission();
        if (err == 0) {
            Serial.printf("[I2C_CAM] Device found at 0x%02X\n", addr);
            found++;
        }
    }
    if (found == 0) {
        Serial.println("[I2C_CAM] No devices found on Bus 1");
    }
}

void lcd_init() {
    Wire.begin(I2C_SDA_LCD, I2C_SCL_LCD, 100000);
    delay(200);
    
    WireCAM.begin(I2C_SDA_CAM, I2C_SCL_CAM);
    WireCAM.setClock(100000);
    delay(500);  // Espera más tiempo para estabilizar

    i2c_scan_bus();

    Wire.beginTransmission(DISPLAY_ADDR);
    uint8_t err_27 = Wire.endTransmission();
    Wire.beginTransmission(DISPLAY_ADDR_ALT);
    uint8_t err_3f = Wire.endTransmission();

    if (err_27 == 0) {
        lcd = &lcd_27;
        Serial.println("[LCD] Found at 0x27");
    } else if (err_3f == 0) {
        lcd = &lcd_3f;
        Serial.println("[LCD] Found at 0x3F");
    } else {
        Serial.println("[LCD] Not found at 0x27 or 0x3F");
        lcd_ready = false;
        return;
    }

    lcd->init();
    lcd->backlight();
    lcd_ready = true;

    lcd->clear();
    lcd->setCursor(0, 0);
    lcd->print("PrismaBot v1.0");
    lcd->setCursor(0, 1);
    lcd->print("Esperando colores...");
    Serial.println("[LCD] Ready");
}

bool cam_present() {
    WireCAM.beginTransmission(CAM_I2C_ADDR);
    return WireCAM.endTransmission() == 0;
}

bool cam_read_color(uint8_t &color) {
    int n = WireCAM.requestFrom((int)CAM_I2C_ADDR, 1);
    if (n != 1 || !WireCAM.available()) return false;
    color = WireCAM.read();
    return color < NUM_COLORS;
}

void lcd_show_color_action(ColorID color, Action action) {
    if (!lcd_ready || lcd == nullptr) return;

    lcd->clear();

    // Row 0: Color
    lcd->setCursor(0, 0);
    lcd->print("Color: ");
    lcd->print(COLOR_NAMES[color]);

    // Row 1: Action
    lcd->setCursor(0, 1);
    lcd->print("Action: ");
    lcd->print(ACTION_NAMES[action]);

    // Row 2: Status
    lcd->setCursor(0, 2);
    lcd->print("Ejecutando...    ");

    // Row 3: Battery/Status
    lcd->setCursor(0, 3);
    lcd->print("I2C OK           ");
}

// ── Setup & Loop ─────────────────────────────────────────────

void setup() {
    Serial.begin(115200);
    delay(1000);
    Serial.println("\n\n=== ESP32 ROBOT CONTROLLER (I2C) ===");
    
    // Init motor pins
    pinMode(MOTOR_A_IN1, OUTPUT);
    pinMode(MOTOR_A_IN2, OUTPUT);
    pinMode(MOTOR_B_IN3, OUTPUT);
    pinMode(MOTOR_B_IN4, OUTPUT);
    motors_stop();
    
    // Init LCD (same I2C bus used to poll ESP32-CAM)
    lcd_init();

    if (cam_present()) {
        Serial.println("[I2C] ESP32-CAM detected at 0x08");
    } else {
        Serial.println("[I2C] WARNING: ESP32-CAM not detected at 0x08");
    }

    Serial.println("Polling color data from ESP32-CAM on I2C...");
}

void loop() {
    uint8_t color_byte = COLOR_NONE;
    if (cam_read_color(color_byte)) {
        last_received_color = color_byte;
    }

    if (last_received_color != (uint8_t)COLOR_NONE &&
        last_received_color != last_processed_color) {
        ColorID color = (ColorID)last_received_color;
        Action action = (Action)COLOR_INSTRUCTION[color];

        Serial.printf("Received: COLOR_%s (%d) -> %s\n",
                      COLOR_NAMES[color], (int)color, ACTION_NAMES[action]);

        lcd_show_color_action(color, action);
        delay(300);
        execute_action(action);

        last_processed_color = last_received_color;
    }

    delay(120);
}
