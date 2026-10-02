#include "PneumaticController.h"
#include "Config.h"

// Constructor Call..
PneumaticController::PneumaticController(
    DoubleSolenoid* steering,
    DoubleSolenoid* resistance,
    DoubleSolenoid* switchValve
) {
    this->steering = steering;
    this->resistance = resistance;
    this->switchValve = switchValve;
}


void PneumaticController::begin() {

    steering->begin();
    resistance->begin();
    switchValve->begin();

    Serial.println("PneumaticController initialized :)");
}


// Handle the separate pneumatic tests choosen by the user
// by clicking the user buttont
void PneumaticController::executeTestCommand(int command) {

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

void PneumaticController::executeRunningCommand() {

    Serial.println("MACHINE RUNNING..");

    // Here goes the main steering rig program logic :)
    switchValve->extend();
    steering->extend();
    resistance->retract();
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

