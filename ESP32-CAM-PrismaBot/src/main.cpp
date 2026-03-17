#include <Arduino.h>
#include "camera_pins.h"
#include "robot_config.h"
#include "color_config.h"
#include "vision.h"
#include "qlearning.h"
#include "robot_control.h"
#include "display_module.h"
#include "rewards.h"

// ── Q-Learning hyper-parameters ───────────────────────────────────
// Adjust these to tune learning speed vs. stability.
static QLearningParams ql_params = {
    .alpha       = 0.1f,   // learning rate  (0 = no learning, 1 = overwrite)
    .gamma       = 0.9f,   // discount factor (0 = myopic, 1 = far-sighted)
    .epsilon     = 0.3f,   // initial exploration rate (0.0–1.0)
    .epsilon_min = 0.05f,  // epsilon decays by ×0.99 each episode to this floor
};

// ── Agent and statistics ──────────────────────────────────────────
static QLearning agent(ql_params);
static float     total_reward  = 0.0f;
static int       step_count    = 0;

// ══════════════════════════════════════════════════════════════════

void setup() {
    Serial.begin(115200);
    Serial.println("\n[PrismaBot] Booting...");

    // Motor GPIO
    robot_init();

    // I2C LCD — optional; system continues if not found
    if (!display_init()) {
        Serial.println("[display] No display found — continuing without.");
    }

    display_message("PrismaBot", "Camera init...");

    // ESP32-CAM
    if (!vision_init()) {
        Serial.println("[vision] Camera init FAILED — halting.");
        display_message("ERROR", "Camera fail!");
        while (true) delay(1000);
    }

    display_message("PrismaBot", "Ready!");
    delay(1000);
    Serial.println("[PrismaBot] Q-Learning loop started.");
}

// ══════════════════════════════════════════════════════════════════

void loop() {
    // ── 1. Observe current state ──────────────────────────────────
    VisionResult vis   = vision_capture();
    ColorID      color = vis.color;
    Position     pos   = vis.position;

    // ── 2. Select action (epsilon-greedy) ─────────────────────────
    Action action = agent.selectAction(color, pos);

    Serial.printf("[EP %d | ST %d] color=%d pos=%d → %s\n",
                  agent.episode(), step_count,
                  (int)color, (int)pos, action_name(action));

    // ── 3. Execute action on the robot ────────────────────────────
    robot_execute(action);

    // ── 4. Observe next state ──────────────────────────────────────
    VisionResult vis_next   = vision_capture();
    ColorID      next_color = vis_next.color;
    Position     next_pos   = vis_next.position;

    // ── 5. Determine episode-ending conditions ─────────────────────
    // out_of_bounds: hook for IR / line sensors (extend here)
    bool out_of_bounds = false;

    // finished: goal colour detected and agent chose the right action
    bool finished = (color == GOAL_COLOR &&
                     (uint8_t)action == COLOR_INSTRUCTION[GOAL_COLOR]);

    // ── 6. Compute reward ─────────────────────────────────────────
    float reward  = compute_reward(color, pos, action,
                                   out_of_bounds, finished);
    total_reward += reward;
    step_count++;

    // ── 7. Q-table update ─────────────────────────────────────────
    agent.update(color, pos, action, reward, next_color, next_pos);

    // ── 8. Refresh LCD ───────────────────────────────────────────
    display_update(color, pos, action,
                   agent.episode(), step_count, total_reward);

    // ── 9. Episode boundary handling ──────────────────────────────
    if (finished || out_of_bounds) {
        Serial.printf("[PrismaBot] Episode %d ended — "
                      "steps=%d  total_reward=%.1f\n",
                      agent.episode(), step_count, total_reward);

        display_message("Episode end", "Resetting...");
        robot_stop();

        agent.newEpisode();
        total_reward = 0.0f;
        step_count   = 0;
        delay(2000);
    }

    delay(VISION_SAMPLE_MS);
}