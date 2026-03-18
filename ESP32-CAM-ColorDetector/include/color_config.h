#pragma once
#include <stdint.h>

// Color identifiers
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

// HSV range descriptor
struct HSVRange {
    uint8_t h_min, h_max;
    uint8_t s_min, s_max;
    uint8_t v_min, v_max;
};

// Per-color HSV ranges
static const HSVRange COLOR_HSV[NUM_COLORS] = {
    {   0,   0,   0,   0,   0,   0 },  // NONE
    {   0,  10, 120, 255, 100, 255 }, // RED
    {  40,  80,  80, 255,  80, 255 }, // GREEN
    {  20,  35, 100, 255, 100, 255 }, // YELLOW
    { 100, 140,  80, 255,  80, 255 }, // BLUE
    {   0, 180,   0,  50, 200, 255 }, // WHITE
    { 140, 170,  80, 255,  80, 255 }, // PINK
    {   0, 180,   0, 255,   0,  50 },  // BLACK
};

static const HSVRange RED_RANGE2 = { 170, 180, 120, 255, 100, 255 };

#define COLOR_DETECT_THRESHOLD 50

// Action enum (mapeo de acciones a ejecutar)
enum Action : uint8_t {
    ACT_STOP        = 0,
    ACT_BACKWARD    = 1,
    ACT_ADVANCE     = 2,
    ACT_TURN_LEFT   = 3,
    ACT_TURN_RIGHT  = 4,
    ACT_TURN_180    = 5,
};

// Color instruction mapping (qué acción ejecutar para cada color)
static const uint8_t COLOR_INSTRUCTION[NUM_COLORS] = {
    ACT_STOP,      // COLOR_NONE
    ACT_TURN_RIGHT, // COLOR_RED
    ACT_STOP,       // COLOR_GREEN
    ACT_BACKWARD,   // COLOR_YELLOW
    ACT_TURN_180,   // COLOR_BLUE
    ACT_ADVANCE,    // COLOR_WHITE
    ACT_BACKWARD,   // COLOR_PINK
    ACT_ADVANCE     // COLOR_BLACK
};

// Nombres para debug
static const char* COLOR_NAMES[NUM_COLORS] = {
    "NONE", "RED", "GREEN", "YELLOW", "BLUE", "WHITE", "PINK", "BLACK"
};

static const char* ACTION_NAMES[6] = {
    "STOP", "BACKWARD", "ADVANCE", "TURN_LEFT", "TURN_RIGHT", "TURN_180"
};
