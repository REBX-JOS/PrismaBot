#include "display_module.h"
#include "robot_config.h"
#include "robot_control.h"  // action_name()
#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// HD44780 20×4 LCD via PCF8574 I2C backpack.
// To use a different size, change LCD_COLS / LCD_ROWS in robot_config.h.
static LiquidCrystal_I2C lcd(DISPLAY_ADDR, LCD_COLS, LCD_ROWS);

static bool s_display_ready = false;

// ── Helper: short colour label ────────────────────────────────────

static const char* color_label(ColorID c) {
    switch (c) {
        case COLOR_RED:    return "RED  ";
        case COLOR_GREEN:  return "GREEN";
        case COLOR_YELLOW: return "YEL  ";
        default:           return "NONE ";
    }
}

static const char* pos_label(Position p) {
    switch (p) {
        case POS_LEFT:  return "L";
        case POS_RIGHT: return "R";
        default:        return "C";
    }
}

// ── Helper: write one full LCD row (pads to LCD_COLS with spaces) ─

static void lcd_print_row(uint8_t row, const char* text) {
    lcd.setCursor(0, row);
    int len = 0;
    for (; text[len] && len < LCD_COLS; len++) {
        lcd.write((uint8_t)text[len]);
    }
    // Pad remainder of the row with spaces to clear old content
    for (; len < LCD_COLS; len++) {
        lcd.write(' ');
    }
}

// ── Initialisation ────────────────────────────────────────────────

bool display_init() {
    Wire.begin(I2C_SDA, I2C_SCL);

    // Probe the I2C bus before handing control to the library
    Wire.beginTransmission(DISPLAY_ADDR);
    if (Wire.endTransmission() != 0) {
        Serial.printf("[display] No I2C device at address 0x%02X.\n",
                      DISPLAY_ADDR);
        s_display_ready = false;
        return false;
    }

    lcd.init();
    lcd.backlight();
    s_display_ready = true;

    char splash[LCD_COLS + 1];
    snprintf(splash, sizeof(splash), "PrismaBot v%s", PRISMABOT_VERSION);
    display_message(splash, "Starting...");
    Serial.println("[display] LCD 20x4 ready.");
    return true;
}

// ── Status screen — 4 rows ────────────────────────────────────────
//
//  Row 0:  "ACT: ADVANCE        "
//  Row 1:  "CLR: GREEN  POS: C  "
//  Row 2:  "EP:  3   ST:  27    "
//  Row 3:  "RWD:  47.0    v1.0.0"

void display_update(ColorID color,  Position pos,
                    Action  action, int      episode,
                    int     steps,  float    reward_total) {
    if (!s_display_ready) return;

    char buf[LCD_COLS + 1];

    // Row 0 — current action
    snprintf(buf, sizeof(buf), "ACT: %-8s", action_name(action));
    lcd_print_row(0, buf);

    // Row 1 — colour and lateral position
    snprintf(buf, sizeof(buf), "CLR: %s POS: %s",
             color_label(color), pos_label(pos));
    lcd_print_row(1, buf);

    // Row 2 — episode and step counter
    snprintf(buf, sizeof(buf), "EP: %-4d ST: %-5d", episode, steps);
    lcd_print_row(2, buf);

    // Row 3 — cumulative reward + version tag
    snprintf(buf, sizeof(buf), "RWD:%-7.1f v%-6s",
             reward_total, PRISMABOT_VERSION);
    lcd_print_row(3, buf);
}

// ── Simple two-line message ───────────────────────────────────────

void display_message(const char* line1, const char* line2) {
    if (!s_display_ready) return;
    lcd.clear();
    if (line1) lcd_print_row(0, line1);
    if (line2) lcd_print_row(1, line2);
}
