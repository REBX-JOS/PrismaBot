#pragma once
#include "qlearning.h"

/*
 * robot_control.h — Motor driver abstraction.
 *
 * Maps high-level Action values to L298N GPIO signals.
 * Timing constants live in robot_config.h; pin assignments too.
 *
 * Extension hook: replace the bodies in robot_control.cpp to support
 * servos, stepper motors, or a UART bridge to a separate MCU.
 */

// Initialise motor GPIO pins (call once in setup())
void        robot_init();

// Execute an action and block until the motion is complete
void        robot_execute(Action action);

// Immediately cut all motor outputs
void        robot_stop();

// Human-readable action name for logging / display
const char* action_name(Action a);
