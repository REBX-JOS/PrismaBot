#pragma once
#include <stdint.h>

// Color IDs (sincronizado con ESP32-CAM)
#define NUM_COLORS 8

enum ColorID : uint8_t {
    COLOR_NONE      = 0,
    COLOR_RED       = 1,
    COLOR_GREEN     = 2,
    COLOR_YELLOW    = 3,
    COLOR_BLUE      = 4,
    COLOR_WHITE     = 5,
    COLOR_PINK      = 6,
    COLOR_BLACK     = 7,
};

// Actions
enum Action : uint8_t {
    ACT_ADVANCE    = 0,  // Avanzar
    ACT_TURN_RIGHT = 1,  // Girar derecha
    ACT_TURN_LEFT  = 2,  // Girar izquierda
    ACT_TURN_180   = 3,  // Girar sobre su eje
    ACT_STOP       = 4,  // Detenerse
    ACT_BACKWARD   = 5   // Retroceder
};

// Color → Action mapping
static const uint8_t COLOR_INSTRUCTION[NUM_COLORS] = {
    0,   // NONE   → ADVANCE
    3,   // RED    → TURN_180
    0,   // GREEN  → ADVANCE
    1,   // YELLOW → TURN_RIGHT
    4,   // BLUE   → STOP
    5,   // WHITE  → BACKWARD
    2,   // PINK   → TURN_LEFT
    5,   // BLACK  → BACKWARD (castigo)
};

static const char* COLOR_NAMES[NUM_COLORS] = {
    "NONE", "RED", "GREEN", "YELLOW", "BLUE", "WHITE", "PINK", "BLACK"
};

static const char* ACTION_NAMES[6] = {
    "ADVANCE", "TURN_R", "TURN_L", "TURN_180", "STOP", "BACKWARD"
};
