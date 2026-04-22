#pragma once

// GPIO pins for NodeMCU ESP32
// Motor A (left wheel)
#define MOTOR_A_IN1 14
#define MOTOR_A_IN2 27

// Motor B (right wheel)
#define MOTOR_B_IN3 26
#define MOTOR_B_IN4 25

// PWM channels
#define MOTOR_A_PWM_CH 0
#define MOTOR_B_PWM_CH 1
#define MOTOR_PWM_FREQ 1000
#define MOTOR_PWM_RES 8


// Speed profiles (0-255)
// SPEED_1_PWM: 50% duty cycle (aprox. 2.5V si Vcc=5V)
#define SPEED_1_PWM 128
// SPEED_2_PWM: 100% duty cycle (5V si Vcc=5V)
#define SPEED_2_PWM 255

// Alternate between speeds every 5 seconds
#define SPEED_SWITCH_INTERVAL_MS 5000
