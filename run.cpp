#include "PneumaticController.h"
#include "Config.h"

// ============================================================
// Automatic run test:
//   RELEASE (all valves stopped)
//   SWITCH LEFT/RIGHT -> 3x steering block
//   flip switch mode -> back to RELEASE ... repeats forever while RUNNING
//
// Each steering block: steer LEFT + resistance RIGHT until the encoder
// angle reaches -STEER_TARGET_DEG, then steer RIGHT + resistance LEFT
// until it reaches +STEER_TARGET_DEG. Closed-loop on the real encoder
// angle, not a fixed time, so strokes always end at the same physical
// angle and nothing drifts cycle over cycle.
//
// Non-blocking (millis()-based): no delay() is used, so encoder
// sampling in loop() is never interrupted.
// ============================================================

void PneumaticController::run() {

    const unsigned long RELEASE_HOLD_MS = Config::Timing::RELEASE_HOLD_MS;
    const unsigned long SWITCH_HOLD_MS  = Config::Timing::SWITCH_PULSE_MS;
    const float TARGET_DEG              = Config::Run::STEER_TARGET_DEG;
    const int BLOCK_REPEATS             = Config::Timing::STEER_BLOCK_REPEATS;

    unsigned long now = millis();

    switch (runPhase) {

        case RUN_RELEASE:
            if (!runPhaseEntered) {
                steering->stop();
                resistance->stop();
                switchValve->stop();
                Serial.println("RELEASE (valves stopped)");
                runPhaseStart = now;
                runPhaseEntered = true;
            }

            if (now - runPhaseStart >= RELEASE_HOLD_MS) {
                runPhase = RUN_SWITCH_PULSE;
                runPhaseEntered = false;
            }
            break;

        case RUN_SWITCH_PULSE:
            if (!runPhaseEntered) {
                if (runSwitchModeLeft) {
                    switchValve->extend();
                    Serial.println("SWITCH LEFT");
                } else {
                    switchValve->retract();
                    Serial.println("SWITCH RIGHT");
                }
                runPhaseStart = now;
                runPhaseEntered = true;
            }

            if (now - runPhaseStart >= SWITCH_HOLD_MS) {
                switchValve->stop();
                runBlockCount = 0;
                runPhase = RUN_STEER_LEFT;
                runPhaseEntered = false;
            }
            break;

        case RUN_STEER_LEFT:
            if (!runPhaseEntered) {
                steering->retract();   // Steer LEFT
                resistance->extend();  // Resistance RIGHT
                Serial.println("Steer LEFT / Resistance RIGHT");
                runPhaseEntered = true;
            }

            if (encoder->angleDeg() <= -TARGET_DEG) {
                runPhase = RUN_STEER_RIGHT;
                runPhaseEntered = false;
            }
            break;

        case RUN_STEER_RIGHT:
            if (!runPhaseEntered) {
                steering->extend();     // Steer RIGHT
                resistance->retract();  // Resistance LEFT
                Serial.println("Steer RIGHT / Resistance LEFT");
                runPhaseEntered = true;
            }

            if (encoder->angleDeg() >= TARGET_DEG) {
                runBlockCount++;

                if (runBlockCount >= BLOCK_REPEATS) {
                    runSwitchModeLeft = !runSwitchModeLeft;  // flip switch mode
                    runPhase = RUN_RELEASE;
                } else {
                    runPhase = RUN_STEER_LEFT;
                }

                runPhaseEntered = false;
            }
            break;
    }
}
