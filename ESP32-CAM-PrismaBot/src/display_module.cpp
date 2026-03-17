#include "display_module.h"
#include "robot_config.h"
#include "robot_control.h"  // action_name()#include <Arduino.h>
#include <Wire.h>
#include <U8g2lib.h>

// SSD1306 128×64 OLED on software I2C so any two GPIOs work as bus.
// To switch to a different display, replace only this constructor.
static U8G2_SSD1306_128X64_NONAME_F_SW_I2C
    u8g2(U8G2_R0, /*clk=*/I2C_SCL, /*data=*/I2C_SDA, U8X8_PIN_NONE);

static bool s_display_ready = false;

// ── Helper: short colour label ────────────────────────────────────

static const char* color_label(ColorID c) {
    switch (c) {
        case COLOR_RED:    return "RED";
        case COLOR_GREEN:  return "GREEN";
        case COLOR_YELLOW: return "YEL";
        default:           return "NONE";
    }
}

static const char* pos_label(Position p) {
    switch (p) {
        case POS_LEFT:  return "L";
        case POS_RIGHT: return "R";
        default:        return "C";
    }
}

// ── Initialisation ────────────────────────────────────────────────

bool display_init() {
    // Probe I2C bus before initialising U8g2
    Wire.begin(I2C_SDA, I2C_SCL);
    Wire.beginTransmission(DISPLAY_ADDR);
    if (Wire.endTransmission() != 0) {
        Serial.println("[display] No I2C device at address 0x3C.");
        s_display_ready = false;
        return false;
    }

    u8g2.begin();
    s_display_ready = true;
    char splash[32];
    snprintf(splash, sizeof(splash), "PrismaBot v%s", PRISMABOT_VERSION);
    display_message(splash, "Starting...");
    Serial.println("[display] OLED ready.");
    return true;
}

// ── Status screen ─────────────────────────────────────────────────

void display_update(ColorID color,  Position pos,
                    Action  action, int      episode,
                    int     steps,  float    reward_total) {
    if (!s_display_ready) return;

    char buf[24];
    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_6x10_tf);

    // Row 1 — action
    snprintf(buf, sizeof(buf), "ACT: %s", action_name(action));
    u8g2.drawStr(0, 10, buf);

    // Row 2 — colour + position
    snprintf(buf, sizeof(buf), "CLR: %-5s POS: %s",
             color_label(color), pos_label(pos));
    u8g2.drawStr(0, 22, buf);

    // Row 3 — episode / step
    snprintf(buf, sizeof(buf), "EP: %-3d  ST: %d", episode, steps);
    u8g2.drawStr(0, 34, buf);

    // Row 4 — cumulative reward
    snprintf(buf, sizeof(buf), "RWD: %.1f", reward_total);
    u8g2.drawStr(0, 46, buf);

    // Row 5 — branding
    u8g2.setFont(u8g2_font_5x7_tf);
    char brand[32];
    snprintf(brand, sizeof(brand), "PrismaBot Q-Agent v%s", PRISMABOT_VERSION);
    u8g2.drawStr(0, 58, brand);

    u8g2.sendBuffer();
}

// ── Simple two-line message ───────────────────────────────────────

void display_message(const char* line1, const char* line2) {
    if (!s_display_ready) return;
    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_8x13_tf);
    if (line1) u8g2.drawStr(0, 20, line1);
    if (line2) u8g2.drawStr(0, 40, line2);
    u8g2.sendBuffer();
}
