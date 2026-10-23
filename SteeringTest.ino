
#include "OptaBlue.h"
#include "Config.h"
#include "DoubleSolenoid.h"
#include "ButtonHandler.h"
#include "PneumaticController.h"
#include "AmtEncoder.h"

using namespace Opta;

// ============================================================
// HARDWARE CONFIGURATION
// ============================================================

DigitalStSolidExpansion stsolidExp;

DoubleSolenoid steering(
    &stsolidExp,
    Config::Pins::STEERING_A,
    Config::Pins::STEERING_B
);

DoubleSolenoid resistance(
    &stsolidExp,
    Config::Pins::RESISTANCE_A,
    Config::Pins::RESISTANCE_B
);

DoubleSolenoid switchValve(
    &stsolidExp,
    Config::Pins::SWITCH_A,
    Config::Pins::SWITCH_B
);

// ============================================================
// CONTROLLERS
// ============================================================

AmtEncoder encoder1(
    "E1",
    Config::Pins::ENC1_A,
    Config::Pins::ENC1_B,
    Config::Encoder::PPR
);

PneumaticController pneumatic(
    &steering,
    &resistance,
    &switchValve,
    &encoder1
);

ButtonHandler buttons(
    Config::Pins::USER_BUTTON,
    Config::Pins::START_BUTTON,
    &stsolidExp
);

AmtEncoder encoder2(
    "E2",
    Config::Pins::ENC2_A,
    Config::Pins::ENC2_B,
    Config::Encoder::PPR
);

// ============================================================
// GENERAL TIMING
// ============================================================

unsigned long lastAnglePrint = 0;
unsigned long lastService = 0;

// ============================================================
// SETUP
// ============================================================

void setup() {
    Serial.begin(Config::SerialPort::BAUD);

    OptaController.begin();
    stsolidExp = OptaController.getExpansion(0);

    pneumatic.begin();
    buttons.begin();

    encoder1.begin();

    if (Config::Encoder::USE_ENCODER2) {
        encoder2.begin();
    }

    Serial.println("System ready.");
    Serial.println("Waiting for START/calibration sequence.");
}

// ============================================================
// LOOP
// ============================================================

void loop() {
    // Keep sampling encoders continuously.
    encoder1.update();

    if (Config::Encoder::USE_ENCODER2) {
        encoder2.update();
    }

    unsigned long now = millis();

    if (now - lastService < 5) {
        return;
    }

    lastService = now;

    buttons.update();

    bool startEvent = buttons.startPressed();

    // Second START: zero exactly here, at whatever position the
    // wheel is in right now (the operator has rotated it by hand
    // during CALIBRATION / LED blinking). Also reset the run() state
    // machine so a leftover phase/timestamp from a previous run can't
    // snap the valves into a new position as soon as RUNNING resumes.
    if (startEvent && buttons.isRunning()) {
        encoder1.zero();

        if (Config::Encoder::USE_ENCODER2) {
            encoder2.zero();
        }

        pneumatic.resetRun();

        Serial.println("E1 ANGLE = 0 DEG");
    }

    // Third START (back to IDLE): release all valves so nothing stays
    // energized/holding pressure between runs.
    else if (startEvent && buttons.isIdle()) {
        pneumatic.test(Config::CMD_STOP);
    }

    // USER button pneumatic test commands.
    int command = buttons.getUserCommand();

    if (command > 0) {
        pneumatic.test(command);
    }

    // Automatic pneumatic operation while running (currently empty:
    // rotation is done by hand for this test).
    if (buttons.isRunning()) {
        pneumatic.run();
    }

    // Live output.
    now = millis();

    if (now - lastAnglePrint >= Config::Encoder::PRINT_MS) {
        lastAnglePrint = now;

        if (buttons.isCalibrating()) {
            // Show the raw X index reading while searching for it by hand.
            Serial.print("Calibration X raw: ");
            Serial.println(analogRead(Config::Pins::ENC1_X));
        } else {
            Serial.print("E1 | Angle: ");
            Serial.println(encoder1.angleDeg(), 2);

            if (Config::Encoder::USE_ENCODER2) {
                Serial.print("E2 | Angle: ");
                Serial.println(encoder2.angleDeg(), 2);
            }
        }
    }
}
