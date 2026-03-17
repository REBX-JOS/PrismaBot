#pragma once
#include <stdint.h>

/*
 * color_config.h — User-facing color range and action mapping.
 *
 * ════════════════════════════════════════════════════════════════
 *  CONFIGURE YOUR COLORS HERE
 *
 *  Use the Calibrador-HSV-Simple tool
 *  (https://github.com/Isaias-Is/Calibrador-HSV-Simple)
 *  to find the exact HSV thresholds for your environment, then
 *  update the COLOR_HSV table below.
 *
 *  Hue   : 0–179   (OpenCV / ESP32 convention — half the 0-360 scale)
 *  Sat   : 0–255
 *  Val   : 0–255
 * ════════════════════════════════════════════════════════════════
 */

// ── Color identifiers ────────────────────────────────────────────
#define NUM_COLORS 4

enum ColorID : uint8_t {
    COLOR_NONE   = 0,
    COLOR_RED    = 1,
    COLOR_GREEN  = 2,
    COLOR_YELLOW = 3
};

// ── HSV range descriptor ─────────────────────────────────────────
struct HSVRange {
    uint8_t h_min, h_max;
    uint8_t s_min, s_max;
    uint8_t v_min, v_max;
};

// ── Per-color HSV ranges (index = ColorID) ───────────────────────
// Red wraps around hue 0/180; a second range (RED_RANGE2) handles
// the upper end and is checked automatically by vision.cpp.
static const HSVRange COLOR_HSV[NUM_COLORS] = {
    // NONE   — placeholder, never matched
    {   0,   0,   0,   0,   0,   0 },
    // RED    — hue ≈ 0-10
    {   0,  10, 120, 255, 100, 255 },
    // GREEN  — hue ≈ 40-80
    {  40,  80,  80, 255,  80, 255 },
    // YELLOW — hue ≈ 20-35
    {  20,  35, 100, 255, 100, 255 },
};

// Second red range (hue wraps near 170-180)
static const HSVRange RED_RANGE2 = { 170, 180, 120, 255, 100, 255 };

// ── Detection sensitivity ────────────────────────────────────────
// Minimum number of matching pixels in a 160×120 frame to consider
// a color "detected".
#define COLOR_DETECT_THRESHOLD  50

// ── Color → instruction action mapping ──────────────────────────
// These are the "correct" actions a perfectly trained agent should
// perform when it sees each color.  Used by the reward function.
//   0 = ACT_ADVANCE   1 = ACT_TURN_RIGHT
//   2 = ACT_TURN_LEFT  3 = ACT_TURN_180   4 = ACT_STOP
static const uint8_t COLOR_INSTRUCTION[NUM_COLORS] = {
    0,   // NONE   → ADVANCE (keep going)
    3,   // RED    → TURN_180
    0,   // GREEN  → ADVANCE (goal — triggers +50 finish reward)
    1,   // YELLOW → TURN_RIGHT
};

// ── Goal color: reaching it ends the episode with +50 reward ─────
#define GOAL_COLOR  COLOR_GREEN
