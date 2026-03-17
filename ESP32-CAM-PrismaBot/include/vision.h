#pragma once
#include "color_config.h"

/*
 * vision.h — Camera capture and HSV color detection module.
 *
 * Call vision_init() once in setup(), then call vision_capture()
 * each time you need the current color/position observation.
 */

// ── Position of the robot relative to the visible path ───────────
#define POS_COUNT 3

enum Position : uint8_t {
    POS_LEFT   = 0,
    POS_CENTER = 1,
    POS_RIGHT  = 2
};

// ── Single observation returned by the camera ─────────────────────
struct VisionResult {
    ColorID  color;     // dominant detected color
    Position position;  // estimated lateral position on path
};

// Initialise the ESP32-CAM camera. Returns true on success.
bool         vision_init();

// Grab a frame and return the current color + position.
VisionResult vision_capture();
