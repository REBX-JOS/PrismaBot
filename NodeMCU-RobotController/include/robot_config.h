#pragma once

// GPIO Pins para NodeMCU ESP32
// Motor A (rueda izquierda)
#define MOTOR_A_IN1  16
#define MOTOR_A_IN2  17

// Motor B (rueda derecha)
#define MOTOR_B_IN3  18
#define MOTOR_B_IN4  19

// I2C LCD (opcional)
#define I2C_SDA      2
#define I2C_SCL      4
#define DISPLAY_ADDR 0x27
#define LCD_COLS     20
#define LCD_ROWS     4

// Timing (ms)
#define MOTOR_ADVANCE_MS  500
#define MOTOR_TURN_90_MS  350
#define MOTOR_TURN_180_MS 700
