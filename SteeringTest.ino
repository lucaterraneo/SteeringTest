
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


ButtonHandler buttons(
    Config::Pins::USER_BUTTON,
    Config::Pins::START_BUTTON,
    &stsolidExp
);

AmtEncoder encoder1(
    "E1",
    Config::Pins::ENC1_A,
    Config::Pins::ENC1_B,
    Config::Encoder::PPR
);

AmtEncoder encoder2(
    "E2",
    Config::Pins::ENC2_A,
    Config::Pins::ENC2_B,
    Config::Encoder::PPR
);

PneumaticController pneumatic(
    &steering,
    &resistance,
    &switchValve,
    &encoder1
);


unsigned long lastService = 0;
unsigned long lastAnglePrint = 0;


void setup() {

    Serial.begin(Config::SerialPort::BAUD);
    OptaController.begin();
    stsolidExp = OptaController.getExpansion(0);

    // begin with all the pneumatic valves off..
    pneumatic.begin();
    buttons.begin();

    // So run()'s blocking waits can still see the START button.
    pneumatic.attachButtons(&buttons);

    encoder1.begin();

    if (Config::Encoder::USE_ENCODER2) {
        encoder2.begin();
    }
    Serial.println("put the backrest in the middle position and press start button again :)");
}


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
    }

    // stop steering test on the third button push
    else if (startEvent && buttons.isIdle()) {
        pneumatic.test(Config::CMD_STOP);
    }

    // USER button pneumatic test commands.
    int command = buttons.getUserCommand();

    if (command > 0) {
        pneumatic.test(command);
    }

    // Automatic pneumatic operation while running
    if (buttons.isRunning()) {
        pneumatic.run();
    }



    // Live output of the angle read, even before start of pneumatic steering test!
    now = millis();

    if (now - lastAnglePrint >= Config::Encoder::PRINT_MS) {
        lastAnglePrint = now;

        Serial.print("Angle: ");
        Serial.print(encoder1.angleDeg(), 2);
        Serial.println(" °");
    }
}
