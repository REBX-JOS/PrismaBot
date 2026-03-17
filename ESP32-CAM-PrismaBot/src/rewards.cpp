#include "rewards.h"

/*
 * compute_reward — Single-call reward shaper for the Q-Learning agent.
 *
 * Parameters:
 *   color         – detected colour at time of action
 *   pos           – detected lateral position at time of action
 *   action        – action the agent chose
 *   out_of_bounds – true when an external sensor confirms the robot
 *                   has left the track (hook for IR / line sensors)
 *   finished      – true when the goal colour was reached correctly
 *
 * Returns the scalar reward to be fed into the Q-update.
 */
float compute_reward(ColorID color,        Position pos,
                     Action  action,       bool     out_of_bounds,
                     bool    finished) {
    // ── Terminal conditions (high magnitude) ─────────────────────
    if (out_of_bounds) return -100.0f;
    if (finished)      return  +50.0f;

    float reward = 0.0f;

    // ── Colour-instruction alignment (+10 / -5) ───────────────────
    uint8_t correct = COLOR_INSTRUCTION[color];
    if ((uint8_t)action == correct) {
        reward += 10.0f;
    } else if (color != COLOR_NONE) {
        // Agent ignored a meaningful colour cue
        reward -= 5.0f;
    }

    // ── Path-following bonus (+1 when centred) ────────────────────
    if (pos == POS_CENTER) {
        reward += 1.0f;
    }

    // ── Inefficiency penalty (-1 for stopping on a clear path) ───
    if (action == ACT_STOP && color == COLOR_NONE) {
        reward -= 1.0f;
    }

    return reward;
}
