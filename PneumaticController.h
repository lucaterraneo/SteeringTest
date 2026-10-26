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

    // Automatic run() state (persists across calls; reset via
    // resetRun() whenever RUNNING is (re-)entered).
    // mode = 1 -> TRADITIONAL, mode = 2 -> STEERBYLEAN
    int mode;

    void steeringRight();
    void steeringLeft();

    void resistanceLeft();
    void resistanceRight();

    void switchOn();
    void switchOff();

    void stopAll();

    // Like delay(ms), but keeps calling encoder->update() the whole
    // time so a steering stroke done with a blocking wait doesn't
    // lose quadrature counts while it's actuating.
    void waitKeepingEncoder(unsigned long ms);

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
