#include "qlearning.h"
#include <Arduino.h>
#include <string.h>

// ── Construction / reset ─────────────────────────────────────────

QLearning::QLearning(QLearningParams params)
    : _params(params),
      _epsilon(params.epsilon),
      _last_action(ACT_ADVANCE),
      _episode(0),
      _steps(0)
{
    reset();
}

void QLearning::reset() {
    memset(_q, 0, sizeof(_q));
    _epsilon     = _params.epsilon;   // restore original exploration rate
    _last_action = ACT_ADVANCE;
    _episode     = 0;
    _steps       = 0;
}

// ── Episode management ───────────────────────────────────────────

void QLearning::newEpisode() {
    _episode++;
    _steps       = 0;
    _last_action = ACT_ADVANCE;

    // Gradually shift from exploration to exploitation.
    // epsilon_min defaults to 0.05 if not set (0 means use 0.05 floor).
    float floor = (_params.epsilon_min > 0.0f) ? _params.epsilon_min : 0.05f;
    if (_epsilon > floor) {
        _epsilon *= 0.99f;          // reduce by ~1% each episode
        if (_epsilon < floor) _epsilon = floor;
    }
}

// ── State encoding ───────────────────────────────────────────────

int QLearning::encodeState(ColorID color, Position pos) const {
    return (int)color * (POS_COUNT * ACT_COUNT)
         + (int)pos   * ACT_COUNT
         + (int)_last_action;
}

// ── Greedy and random action selection ───────────────────────────

Action QLearning::greedyAction(int state) const {
    int   best_idx = 0;
    float best_val = _q[state][0];
    for (int a = 1; a < ACT_COUNT; a++) {
        if (_q[state][a] > best_val) {
            best_val = _q[state][a];
            best_idx = a;
        }
    }
    return (Action)best_idx;
}

Action QLearning::randomAction() const {
    return (Action)random(ACT_COUNT);
}

// ── Epsilon-greedy action selection ──────────────────────────────

Action QLearning::selectAction(ColorID color, Position pos) {
    int    state   = encodeState(color, pos);
    Action chosen;

    if ((float)random(1000) / 1000.0f < _epsilon) {
        chosen = randomAction();
    } else {
        chosen = greedyAction(state);
    }

    _last_action = chosen;
    _steps++;
    return chosen;
}

// ── Bellman Q-update ─────────────────────────────────────────────

void QLearning::update(ColorID color,      Position pos,
                       Action  action,     float    reward,
                       ColorID next_color, Position next_pos) {
    int s = encodeState(color, pos);

    // Build next state with the action just taken as the new "last action"
    Action saved    = _last_action;
    _last_action    = action;
    int    ns       = encodeState(next_color, next_pos);
    _last_action    = saved;

    // max Q(s')
    float max_next = _q[ns][0];
    for (int a = 1; a < ACT_COUNT; a++) {
        if (_q[ns][a] > max_next) max_next = _q[ns][a];
    }

    // Q(s,a) += α * [r + γ * max Q(s') − Q(s,a)]
    _q[s][action] += _params.alpha *
                     (reward + _params.gamma * max_next - _q[s][action]);
}

// ── Read-only access ─────────────────────────────────────────────

float QLearning::getQ(int state, int action) const {
    if (state  < 0 || state  >= NUM_STATES) return 0.0f;
    if (action < 0 || action >= ACT_COUNT)  return 0.0f;
    return _q[state][action];
}
