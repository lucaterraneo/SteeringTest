#include "PneumaticController.h"
#include "Config.h"

// Constructor Call..
PneumaticController::PneumaticController(
    DoubleSolenoid* steering,
    DoubleSolenoid* resistance,
    DoubleSolenoid* switchValve,
    AmtEncoder* encoder
) {
    this->steering = steering;
    this->resistance = resistance;
    this->switchValve = switchValve;
    this->encoder = encoder;

    buttons = nullptr;
    encoder2 = nullptr;
    mode = 1;
}

void PneumaticController::attachButtons(ButtonHandler* buttons) {
    this->buttons = buttons;
}

void PneumaticController::attachEncoder2(AmtEncoder* encoder2) {
    this->encoder2 = encoder2;
}

// Call this whenever RUNNING is (re-)entered (right next to
// encoder1.zero() in SteeringTest.ino), so a leftover phase/timestamp
// from a previous run can't snap the valves into a new position as
// soon as the loop resumes.
void PneumaticController::resetRun() {
    mode = 2;
}

void PneumaticController::begin() {

    steering->begin();
    resistance->begin();
    switchValve->begin();

    Serial.println("Pneumatic valves initialized in OFF state!");
}

////////////////////////////////////////////////////////////
// Handle the separate pneumatic tests choosen by the user
////////////////////////////////////////////////////////////
// by clicking the user button.
void PneumaticController::test(int command) {

    switch (command) {

        case Config::CMD_STEERING_RIGHT:
            steeringRight();
            break;

        case Config::CMD_STEERING_LEFT:
            steeringLeft();
            break;

        case Config::CMD_RESISTANCE_LEFT:
            resistanceLeft();
            break;

        case Config::CMD_RESISTANCE_RIGHT:
            resistanceRight();
            break;

        case Config::CMD_SWITCH_ON:
            switchOn();
            break;

        case Config::CMD_SWITCH_OFF:
            switchOff();
            break;

        default:
            stopAll();
            break;
    }
}

void PneumaticController::steeringRight() {

    resistance->stop();
    switchValve->stop();
    steering->extend();

    Serial.println("STEERING RIGHT");
}

void PneumaticController::steeringLeft() {

    resistance->stop();
    switchValve->stop();
    steering->retract();

    Serial.println("STEERING LEFT");
}

void PneumaticController::resistanceLeft() {

    steering->stop();
    switchValve->stop();
    resistance->extend();

    Serial.println("RESISTANCE LEFT");
}

void PneumaticController::resistanceRight() {

    steering->stop();
    switchValve->stop();
    resistance->retract();

    Serial.println("RESISTANCE RIGHT");
}

void PneumaticController::switchOn() {

    steering->stop();
    resistance->stop();
    switchValve->extend();
    delay(Config::Timing::SWITCH_PULSE_MS);
    switchValve->stop();

    Serial.println("SWITCH ON");
}

void PneumaticController::switchOff() {

    steering->stop();
    resistance->stop();
    switchValve->retract();
    delay(Config::Timing::SWITCH_PULSE_MS);
    switchValve->stop();

    Serial.println("SWITCH OFF");
}

void PneumaticController::stopAll() {
    steering->stop();
    resistance->stop();
    switchValve->stop();

    Serial.println("ALL PNEUMATICS STOPPED");
}
