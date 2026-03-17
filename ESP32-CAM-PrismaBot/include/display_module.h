#pragma once
#include "color_config.h"
#include "qlearning.h"

/*
 * display_module.h — I2C LCD 20×4 (HD44780 + PCF8574 backpack) interface.
 *
 * Requires the LiquidCrystal_I2C library (marcoschwartz/LiquidCrystal_I2C).
 * Pins and address are configured in robot_config.h (I2C_SDA / I2C_SCL /
 * DISPLAY_ADDR).  Most PCF8574 backpacks ship with address 0x27; if the
 * display is not found, try 0x3F.
 *
 * Extension hook: to support a different size LCD, adjust LCD_COLS /
 * LCD_ROWS in robot_config.h — no other file needs to change.
 */

// Probe the I2C bus and initialise the display.
// Returns true if a display was found; the rest of the system
// continues normally even when this returns false.
bool display_init();

// Refresh the full status screen (all 4 rows)
void display_update(ColorID color,  Position pos,
                    Action  action, int      episode,
                    int     steps,  float    reward_total);

// Show a two-line message (e.g. error / startup splash)
void display_message(const char* line1, const char* line2 = nullptr);
