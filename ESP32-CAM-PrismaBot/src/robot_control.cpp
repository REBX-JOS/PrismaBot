#include "robot_control.h"
#include "robot_config.h"
#include <Arduino.h>

// ── Initialisation ───────────────────────────────────────────────

void robot_init() {
    pinMode(MOTOR_A_IN1, OUTPUT);
    pinMode(MOTOR_A_IN2, OUTPUT);
    pinMode(MOTOR_B_IN3, OUTPUT);
    pinMode(MOTOR_B_IN4, OUTPUT);
    robot_stop();
}

// ── Internal motor primitives ────────────────────────────────────

static void motors_forward() {
    digitalWrite(MOTOR_A_IN1, HIGH);
    digitalWrite(MOTOR_A_IN2, LOW);
    digitalWrite(MOTOR_B_IN3, HIGH);
    digitalWrite(MOTOR_B_IN4, LOW);
}

// In-place turn: left motor forward, right motor backward
static void motors_turn_right() {
    digitalWrite(MOTOR_A_IN1, HIGH);
    digitalWrite(MOTOR_A_IN2, LOW);
    digitalWrite(MOTOR_B_IN3, LOW);
    digitalWrite(MOTOR_B_IN4, HIGH);
}

// In-place turn: left motor backward, right motor forward
static void motors_turn_left() {
    digitalWrite(MOTOR_A_IN1, LOW);
    digitalWrite(MOTOR_A_IN2, HIGH);
    digitalWrite(MOTOR_B_IN3, HIGH);
    digitalWrite(MOTOR_B_IN4, LOW);
}

// ── Public API ───────────────────────────────────────────────────

void robot_stop() {
    digitalWrite(MOTOR_A_IN1, LOW);
    digitalWrite(MOTOR_A_IN2, LOW);
    digitalWrite(MOTOR_B_IN3, LOW);
    digitalWrite(MOTOR_B_IN4, LOW);
}

void robot_execute(Action action) {
    switch (action) {
        case ACT_ADVANCE:
            motors_forward();
            delay(MOTOR_ADVANCE_MS);
            robot_stop();
            break;

        case ACT_TURN_RIGHT:
            motors_turn_right();
            delay(MOTOR_TURN_90_MS);
            robot_stop();
            break;

        case ACT_TURN_LEFT:
            motors_turn_left();
            delay(MOTOR_TURN_90_MS);
            robot_stop();
            break;

        case ACT_TURN_180:
            // 180° = two consecutive 90° turns in the same direction
            motors_turn_right();
            delay(MOTOR_TURN_180_MS);
            robot_stop();
            break;

        case ACT_STOP:
        default:
            robot_stop();
            break;
    }
}

const char* action_name(Action a) {
    switch (a) {
        case ACT_ADVANCE:    return "ADVANCE";
        case ACT_TURN_RIGHT: return "TURN_R";
        case ACT_TURN_LEFT:  return "TURN_L";
        case ACT_TURN_180:   return "TURN_180";
        case ACT_STOP:       return "STOP";
        default:             return "?";
    }
}
