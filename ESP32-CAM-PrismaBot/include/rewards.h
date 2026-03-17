#pragma once
#include "color_config.h"
#include "qlearning.h"

/*
 * rewards.h — Reward function for the Q-Learning agent.
 *
 * Reward scheme
 * ─────────────
 *  +50   episode finished (goal color reached with correct action)
 * −100   robot left the path boundaries
 *  +10   action matches the color's instruction (correct behaviour)
 *   +1   robot is centred on the path (staying in-limits bonus)
 *   −5   action does NOT match the color instruction (wrong move)
 *   −1   unnecessary STOP when path is clear (inefficiency penalty)
 *    0   otherwise
 *
 * Extension hook: add new conditions (e.g. battery level, obstacle
 * proximity) by editing compute_reward() in rewards.cpp.
 */

float compute_reward(ColorID color,        Position pos,
                     Action  action,       bool     out_of_bounds,
                     bool    finished);
