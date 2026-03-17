#pragma once
#include "color_config.h"
#include "qlearning.h"

/*
 * display_module.h — I2C OLED display (SSD1306 128×64) interface.
 *
 * Requires the U8g2 library (olikraus/U8g2).
 * Pins are configured in robot_config.h (I2C_SDA / I2C_SCL).
 *
 * Extension hook: swap the U8g2 constructor in display_module.cpp
 * to support any display U8g2 knows about (e.g. 128×32 OLED, LCD).
 */

// Probe the I2C bus and initialise the display.
// Returns true if a display was found; the rest of the system
// continues normally even when this returns false.
bool display_init();

// Refresh the full status screen
void display_update(ColorID color,  Position pos,
                    Action  action, int      episode,
                    int     steps,  float    reward_total);

// Show a two-line message (e.g. error / startup splash)
void display_message(const char* line1, const char* line2 = nullptr);
