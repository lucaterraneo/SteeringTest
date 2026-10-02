#include "OptaBlue.h"
#include "Config.h"
#include "DoubleSolenoid.h"
#include "ButtonHandler.h"
#include "PneumaticController.h"

using namespace Opta;

// ============================================================
// HARDWARE CONFIGURATION
// ============================================================
DigitalStSolidExpansion stsolidExp;

// Solid State Relais (SSR) mapping
DoubleSolenoid steering(&stsolidExp, Config::Pins::STEERING_A, Config::Pins::STEERING_B);
DoubleSolenoid resistance(&stsolidExp, Config::Pins::RESISTANCE_A, Config::Pins::RESISTANCE_B);
DoubleSolenoid switchValve(&stsolidExp, Config::Pins::SWITCH_A, Config::Pins::SWITCH_B);


// ============================================================
// CONTROLLERS
// ============================================================
PneumaticController pneumatic(
    &steering,
    &resistance,
    &switchValve
);

ButtonHandler buttons(
    Config::Pins::USER_BUTTON,
    Config::Pins::START_BUTTON,
    &stsolidExp,
    Config::Pins::CALIBRATION
);


// ============================================================
// SETUP
// ============================================================
void setup() {

    Serial.begin(Config::SerialPort::BAUD);

    // Initialize Opta and expansion
    OptaController.begin();
    stsolidExp = OptaController.getExpansion(0);

    pneumatic.begin();
    buttons.begin();
}


// ============================================================
// LOOP
// ============================================================
void loop() {

    buttons.update();

    // USER button test
    int command = buttons.getUserCommand();

    if (command > 0) {
        pneumatic.executeTestCommand(command);
    }

    // IDEC START button
    if (buttons.isRunning()) {
        pneumatic.executeRunningCommand();
    }
}
