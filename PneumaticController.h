#ifndef PNEUMATIC_CONTROLLER_H
#define PNEUMATIC_CONTROLLER_H

#include <Arduino.h>
#include "DoubleSolenoid.h"

class PneumaticController {

private:

    DoubleSolenoid* steering;
    DoubleSolenoid* resistance;
    DoubleSolenoid* switchValve;

    bool switchPulseActive;
    unsigned long switchPulseStartTime;

    void steeringRight();
    void steeringLeft();

    void resistanceLeft();
    void resistanceRight();

    void switchOn();
    void switchOff();

    void stopAll();
    void handleSwitchPulse();

public:

    // Constructor
    PneumaticController(
        DoubleSolenoid* steering,
        DoubleSolenoid* resistance,
        DoubleSolenoid* switchValve
    );

    void begin();
    void update();

    void executeTestCommand(int command);
    void executeRunningCommand();
};

#endif

