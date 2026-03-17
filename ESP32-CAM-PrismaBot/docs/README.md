# PrismaBot — ESP32-CAM Q-Learning Visual Navigation Agent

A **plug-and-play modular system** that teaches an ESP32-CAM robot to
navigate a colour-coded path using **Q-Learning reinforcement learning**.

---

## Table of Contents

1. [Overview](#overview)
2. [Hardware Requirements](#hardware-requirements)
3. [Wiring](#wiring)
4. [Software Setup](#software-setup)
5. [Configuring Colours](#configuring-colours)
6. [Configuring Actions](#configuring-actions)
7. [Running the Agent](#running-the-agent)
8. [Module Reference](#module-reference)
9. [Extension Hooks](#extension-hooks)

---

## Overview

```
Camera frame
     │
     ▼
[ vision.cpp ]  ──→  ColorID + Position
                              │
                              ▼
                    [ qlearning.cpp ]  ──→  Action
                              │
                    ┌─────────┴──────────┐
                    ▼                    ▼
           [ robot_control.cpp ]  [ rewards.cpp ]
                    │                    │
                    ▼                    ▼
             Motor outputs          Q-table update
                              │
                              ▼
                    [ display_module.cpp ]
                       OLED status screen
```

### Q-Learning State Space

| Dimension     | Values                             | Count |
|---------------|------------------------------------|-------|
| Detected colour | NONE / RED / GREEN / YELLOW      | 4     |
| Lateral position | LEFT / CENTER / RIGHT           | 3     |
| Last action   | ADVANCE / TURN_R / TURN_L / TURN_180 / STOP | 5 |
| **Total states** |                                | **60** |

### Actions

| ID | Name       | Description                        |
|----|------------|------------------------------------|
| 0  | ADVANCE    | Drive straight for `MOTOR_ADVANCE_MS` ms |
| 1  | TURN_RIGHT | In-place 90° right turn            |
| 2  | TURN_LEFT  | In-place 90° left turn             |
| 3  | TURN_180   | In-place 180° turn                 |
| 4  | STOP       | Cut motor outputs                  |

### Reward Scheme

| Condition                          | Reward |
|------------------------------------|--------|
| Goal colour reached (GREEN + ADVANCE) | **+50** |
| Robot left the track              | **−100** |
| Action matches colour instruction  | +10    |
| Robot centred on path              | +1     |
| Action ignores colour signal       | −5     |
| Unnecessary STOP on clear path     | −1     |

---

## Hardware Requirements

- **AI Thinker ESP32-CAM** (OV2640 camera)
- **L298N dual H-bridge motor driver**
- **Two DC motors** (left and right wheel)
- **SSD1306 128×64 OLED display** (I2C, address 0x3C)
- 5 V power supply for motors; 3.3 V logic for ESP32-CAM
- USB-to-TTL adapter for programming (ESP32-CAM has no USB port)

---

## Wiring

### Motor Driver → ESP32-CAM

| L298N pin | ESP32-CAM GPIO | Notes           |
|-----------|----------------|-----------------|
| IN1       | GPIO 14        | Left motor fwd  |
| IN2       | GPIO 15        | Left motor bwd  |
| IN3       | GPIO 12        | Right motor fwd |
| IN4       | GPIO 13        | Right motor bwd |
| GND       | GND            |                 |
| +5V (logic)| 3.3 V        | or external 5 V |

### OLED Display → ESP32-CAM

| OLED pin | ESP32-CAM GPIO | Notes                    |
|----------|----------------|--------------------------|
| SDA      | GPIO 2         | Shared with status LED   |
| SCL      | GPIO 4         | Shared with flash LED    |
| VCC      | 3.3 V          |                          |
| GND      | GND            |                          |

> **Note:** All GPIO assignments can be changed in
> `include/robot_config.h` without touching any other file.

---

## Software Setup

1. Install [PlatformIO](https://platformio.org/) inside VS Code.
2. Open the `ESP32-CAM-PrismaBot` folder as a PlatformIO project.
3. PlatformIO will automatically install the **U8g2** library listed
   in `platformio.ini`.
4. Connect the ESP32-CAM via a USB-to-TTL adapter with the IO0 pin
   pulled to GND during upload (boot mode).
5. Click **Upload** in PlatformIO.
6. Open the Serial Monitor at **115 200 baud** to watch the agent log.

---

## Configuring Colours

Open **`include/color_config.h`** and update the `COLOR_HSV` table:

```cpp
static const HSVRange COLOR_HSV[NUM_COLORS] = {
    // NONE — placeholder
    {   0,   0,   0,   0,   0,   0 },
    // RED   (hue ≈ 0-10)
    {   0,  10, 120, 255, 100, 255 },
    // GREEN (hue ≈ 40-80)
    {  40,  80,  80, 255,  80, 255 },
    // YELLOW (hue ≈ 20-35)
    {  20,  35, 100, 255, 100, 255 },
};
```

### Using the HSV Calibrator

1. Clone [Isaias-Is/Calibrador-HSV-Simple](https://github.com/Isaias-Is/Calibrador-HSV-Simple)
   on a PC connected to the same network as the ESP32-CAM webserver
   (or capture a photo of the coloured markers).
2. Load the image and use the sliders to find H/S/V min-max for each colour.
3. Copy the values into `COLOR_HSV` above and reflash.

> Red wraps around hue 0 / 180.  A second range (`RED_RANGE2`) covers
> hues 170-180; both are checked automatically by `vision.cpp`.

---

## Configuring Actions

Edit the `COLOR_INSTRUCTION` array in `include/color_config.h`:

```cpp
static const uint8_t COLOR_INSTRUCTION[NUM_COLORS] = {
    0,   // NONE   → ADVANCE
    3,   // RED    → TURN_180
    0,   // GREEN  → ADVANCE  ← goal colour
    1,   // YELLOW → TURN_RIGHT
};
```

To change the goal colour (triggers the +50 finish reward):

```cpp
#define GOAL_COLOR  COLOR_GREEN   // change to any ColorID
```

---

## Running the Agent

After flashing:

1. Place the robot on the colour-coded track.
2. Power on — the OLED shows **"PrismaBot / Starting…"** then
   **"PrismaBot / Ready!"**.
3. The agent begins exploring immediately (epsilon = 0.30 initially).
4. Each episode ends when the goal colour is reached or the
   `out_of_bounds` hook fires.
5. Epsilon decays 1 % per episode, gradually shifting the agent
   toward exploitation of learned Q-values.
6. Serial Monitor shows per-step logs:

```
[EP 0 | ST 12] color=2 pos=1 → ADVANCE
[EP 0 | ST 13] color=3 pos=1 → TURN_R
[PrismaBot] Episode 0 ended — steps=14  total_reward=47.0
```

---

## Module Reference

| File                       | Responsibility                                    |
|----------------------------|---------------------------------------------------|
| `include/camera_pins.h`    | AI Thinker ESP32-CAM GPIO definitions             |
| `include/robot_config.h`   | Motor pins, I2C pins, timing constants            |
| `include/color_config.h`   | **User config** — HSV ranges, action mapping      |
| `include/vision.h`         | Camera init + colour/position detection API       |
| `include/qlearning.h`      | Q-Learning agent class                            |
| `include/robot_control.h`  | Motor driver abstraction                          |
| `include/display_module.h` | OLED display API                                  |
| `include/rewards.h`        | Reward function API                               |
| `src/vision.cpp`           | RGB565 → HSV conversion, frame analysis           |
| `src/qlearning.cpp`        | Epsilon-greedy selection, Bellman update          |
| `src/robot_control.cpp`    | L298N GPIO sequences + timing                     |
| `src/display_module.cpp`   | U8g2 SSD1306 rendering                            |
| `src/rewards.cpp`          | Scalar reward computation                         |
| `src/main.cpp`             | `setup()` / `loop()` — wires all modules together |

---

## Extension Hooks

### Add more sensors (IR / ultrasonic)

In `src/main.cpp`, populate the `out_of_bounds` flag:

```cpp
// Example: digital IR line sensor on GPIO 16
bool out_of_bounds = (digitalRead(16) == LOW);
```

### Add more colours / actions

1. Add a new `ColorID` enum value and its `HSVRange` in `color_config.h`.
2. Add the corresponding `COLOR_INSTRUCTION` entry.
3. Add a new `Action` enum value and implement its motor sequence in
   `robot_control.cpp`.
4. Update `NUM_COLORS` / `ACT_COUNT` and recompile.

### Change the reward function

Edit **`src/rewards.cpp`** only — the signature is stable and no other
file needs modification.

### Persist the Q-table across reboots

Write `agent.getQ(state, action)` values to NVS (non-volatile storage)
using the `Preferences` library after each episode, and restore them in
`setup()`.

### Swap the display

Replace only the `U8G2_SSD1306_128X64_NONAME_F_SW_I2C` constructor in
`src/display_module.cpp` with the U8g2 variant that matches your panel.
The rest of the code is display-agnostic.
