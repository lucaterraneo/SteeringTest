#ifndef PNEUMATIC_CONTROLLER_H
#define PNEUMATIC_CONTROLLER_H

#include <Arduino.h>
#include "DoubleSolenoid.h"
#include "AmtEncoder.h"

class PneumaticController {

private:

    DoubleSolenoid* steering;
    DoubleSolenoid* resistance;
    DoubleSolenoid* switchValve;
    AmtEncoder* encoder;

    bool switchPulseActive;
    unsigned long switchPulseStartTime;

    // Automatic run() state (persists across calls; must be explicitly
    // reset via resetRun() whenever RUNNING is (re-)entered, otherwise
    // a stale phaseStart from a previous run would look "timed out"
    // and snap the valves into a new position immediately).
    enum RunPhase {
        RUN_RELEASE,
        RUN_SWITCH_PULSE,
        RUN_STEER_LEFT,
        RUN_STEER_RIGHT
    };

    RunPhase runPhase;
    bool runPhaseEntered;

    // true = switch LEFT, false = switch RIGHT. Flipped every time a
    // full 3x steering block finishes.
    bool runSwitchModeLeft;

    int runBlockCount;
    unsigned long runPhaseStart;

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
        DoubleSolenoid* switchValve,
        AmtEncoder* encoder
    );

    void begin();
    void update();

    void test(int command);
    void run();
    void resetRun();
};

#endif
