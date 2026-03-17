#pragma once

// Firmware version — shown on LCD splash and in Serial output
#define PRISMABOT_VERSION "1.0.0"

/*
 * robot_config.h — Hardware pin assignments and timing constants.
 *
 * Edit this file to match your actual wiring.
 * All GPIO numbers refer to the AI Thinker ESP32-CAM board.
 *
 * Available GPIOs when SD-card is NOT used:
 *   GPIO 2  (onboard status LED, active-low)
 *   GPIO 4  (onboard flash LED,  active-high)
 *   GPIO 12, 13, 14, 15  (SD-card data pins when unused)
 */

// ── Motor A — left wheel (L298N IN1 / IN2) ───────────────────────
#define MOTOR_A_IN1   14
#define MOTOR_A_IN2   15

// ── Motor B — right wheel (L298N IN3 / IN4) ──────────────────────
#define MOTOR_B_IN3   12
#define MOTOR_B_IN4   13

// ── I2C LCD display (HD44780 20×4 via PCF8574 I2C backpack) ──────
// GPIO 2 / 4 are shared with onboard LEDs; they still work as I2C.
#define I2C_SDA        2
#define I2C_SCL        4
#define DISPLAY_ADDR   0x27   // PCF8574 backpack default; try 0x3F if not found
#define LCD_COLS       20     // characters per row
#define LCD_ROWS        4     // number of rows

// ── Camera settings ───────────────────────────────────────────────
#define CAM_XCLK_FREQ_HZ  20000000      // 20 MHz
#define CAM_FRAME_SIZE    FRAMESIZE_QQVGA  // 160×120 — fast for color detection
#define CAM_PIXEL_FORMAT  PIXFORMAT_RGB565

// ── Robot motion timing (milliseconds) ───────────────────────────
#define MOTOR_ADVANCE_MS   500    // duration of one forward step
#define MOTOR_TURN_90_MS   350    // duration for a 90° in-place turn
#define MOTOR_TURN_180_MS  700    // duration for a 180° turn

// ── Vision loop cadence ───────────────────────────────────────────
#define VISION_SAMPLE_MS   100    // delay between camera captures
