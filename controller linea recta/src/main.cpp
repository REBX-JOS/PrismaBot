#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include "robot_config.h"
#include "oled_config.h"

Adafruit_SSD1306 oled(OLED_WIDTH, OLED_HEIGHT, &Wire, -1);

enum SpeedMode {
    SPEED_MODE_1 = 0,
    SPEED_MODE_2 = 1
};

SpeedMode currentMode = SPEED_MODE_1;
unsigned long lastSwitchMs = 0;
bool oledReady = false;

// INVERTIR LOGICA: "adelante" es reversa real, "reversa" es avance real
// Toggle para invertir sentido: true = frente (como está ahora), false = reversa
bool sentido_frente = true;

void motors_forward(uint8_t pwmLeft, uint8_t pwmRight) {
    if (sentido_frente) {
        // Frente (como está ahora)
        digitalWrite(MOTOR_A_IN2, LOW);
        ledcWrite(MOTOR_A_PWM_CH, pwmLeft); // PWM en IN1
        digitalWrite(MOTOR_B_IN4, LOW);
        ledcWrite(MOTOR_B_PWM_CH, pwmRight); // PWM en IN3
    } else {
        // Reversa (invierte lógica)
        digitalWrite(MOTOR_A_IN1, LOW);
        ledcWrite(MOTOR_A_PWM_CH, 0);
        digitalWrite(MOTOR_A_IN2, HIGH);
        digitalWrite(MOTOR_B_IN3, LOW);
        ledcWrite(MOTOR_B_PWM_CH, 0);
        digitalWrite(MOTOR_B_IN4, HIGH);
    }
}

void motors_stop() {
    // Apaga ambos motores
    ledcWrite(MOTOR_A_PWM_CH, 0); // PWM a 0
    ledcWrite(MOTOR_B_PWM_CH, 0);
    digitalWrite(MOTOR_A_IN2, LOW);
    digitalWrite(MOTOR_B_IN4, LOW);
}

void oled_show_mode(SpeedMode mode, uint8_t pwm) {
    if (!oledReady) {
        return;
    }

    oled.clearDisplay();
    oled.setTextSize(1);
    oled.setTextColor(SSD1306_WHITE);
    oled.setCursor(0, 0);
    oled.println("Controller recta");

    oled.setTextSize(2);
    oled.setCursor(0, 18);
    if (mode == SPEED_MODE_1) {
        oled.println("Velocidad 1");
    } else {
        oled.println("Velocidad 2");
    }

    oled.setTextSize(1);
    oled.setCursor(0, 50);
    oled.print("PWM: ");
    oled.print(pwm);
    oled.display();
}

void apply_mode(SpeedMode mode) {
    uint8_t pwm = (mode == SPEED_MODE_1) ? SPEED_1_PWM : SPEED_2_PWM;

    motors_forward(pwm, pwm);
    oled_show_mode(mode, pwm);

    Serial.print("[MODE] ");
    if (mode == SPEED_MODE_1) {
        Serial.println("Velocidad 1");
    } else {
        Serial.println("Velocidad 2");
    }

    // Estado de motores
    Serial.print("MOTORA: ");
    if (pwm > 0) {
        Serial.print("ON, PWM=");
        Serial.println(pwm);
    } else {
        Serial.println("OFF");
    }
    Serial.print("MOTORB: ");
    if (pwm > 0) {
        Serial.print("ON, PWM=");
        Serial.println(pwm);
    } else {
        Serial.println("OFF");
    }
    Serial.println("----------------------");
}


void setup() {
    Serial.begin(115200);
    delay(300);
    Serial.println("\n=== Controller Linea Recta ===");

    pinMode(MOTOR_A_IN1, OUTPUT);
    pinMode(MOTOR_A_IN2, OUTPUT);
    pinMode(MOTOR_B_IN3, OUTPUT);
    pinMode(MOTOR_B_IN4, OUTPUT);

    ledcSetup(MOTOR_A_PWM_CH, MOTOR_PWM_FREQ, MOTOR_PWM_RES);
    ledcSetup(MOTOR_B_PWM_CH, MOTOR_PWM_FREQ, MOTOR_PWM_RES);
    ledcAttachPin(MOTOR_A_IN1, MOTOR_A_PWM_CH); // PWM solo en IN1
    ledcAttachPin(MOTOR_B_IN3, MOTOR_B_PWM_CH); // PWM solo en IN3

    motors_stop();

    Wire.begin(OLED_SDA, OLED_SCL);
    if (!oled.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) {
        Serial.println("[OLED] Init failed");
        oledReady = false;
    } else {
        oledReady = true;
    }

    currentMode = SPEED_MODE_1;
    apply_mode(currentMode);
    lastSwitchMs = millis();
}

void loop() {
    const unsigned long now = millis();

    // Cambia velocidad cada 1 segundo
    if (now - lastSwitchMs >= 3000) {
        currentMode = (currentMode == SPEED_MODE_1) ? SPEED_MODE_2 : SPEED_MODE_1;
        apply_mode(currentMode);
        lastSwitchMs = now;
    }
}
