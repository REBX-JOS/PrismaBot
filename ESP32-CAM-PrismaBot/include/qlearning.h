#pragma once
#include "vision.h"   // Position, POS_COUNT
#include <stdint.h>

/*
 * qlearning.h — Tabular Q-Learning agent.
 *
 * State space  : ColorID (4) × Position (3) × last Action (5) = 60 states
 * Action space : 5 discrete actions
 *
 * The agent learns a Q-table mapping (state, action) → expected return.
 * Action selection is epsilon-greedy; epsilon decays each episode.
 */

// ── Discrete actions ─────────────────────────────────────────────
#define ACT_COUNT 5

enum Action : uint8_t {
    ACT_ADVANCE    = 0,
    ACT_TURN_RIGHT = 1,
    ACT_TURN_LEFT  = 2,
    ACT_TURN_180   = 3,
    ACT_STOP       = 4
};

// Number of Q-table states  = NUM_COLORS * POS_COUNT * ACT_COUNT
#define NUM_STATES  (NUM_COLORS * POS_COUNT * ACT_COUNT)   // 4 * 3 * 5 = 60

// ── Hyper-parameters struct ──────────────────────────────────────
struct QLearningParams {
    float alpha;    // learning rate        (0–1)
    float gamma;    // discount factor      (0–1)
    float epsilon;  // initial exploration  (0–1), decays per episode
    float epsilon_min; // floor for epsilon decay (default 0.05)
};

// ── Q-Learning agent class ───────────────────────────────────────
class QLearning {
public:
    explicit QLearning(QLearningParams params);

    // Reset Q-table and counters (call between training runs)
    void   reset();

    // Begin a new episode (increments counter, decays epsilon)
    void   newEpisode();

    // Epsilon-greedy action selection given current observation
    Action selectAction(ColorID color, Position pos);

    // Bellman update: Q(s,a) += α · [r + γ · max Q(s') − Q(s,a)]
    void   update(ColorID color,      Position pos,
                  Action  action,     float    reward,
                  ColorID next_color, Position next_pos);

    // Read-only Q-value access
    float  getQ(int state, int action) const;

    // Encode (color, pos, last_action) into a flat state index
    int    encodeState(ColorID color, Position pos) const;

    // Accessors
    Action lastAction() const { return _last_action; }
    int    episode()    const { return _episode; }
    int    steps()      const { return _steps; }
    float  epsilon()     const { return _epsilon; }
    float  epsilon_init() const { return _params.epsilon; }

private:
    float           _q[NUM_STATES][ACT_COUNT];
    QLearningParams _params;
    float           _epsilon;     // current (decaying) exploration rate
    Action          _last_action;
    int             _episode;
    int             _steps;

    Action greedyAction(int state) const;
    Action randomAction() const;
};
