#ifndef PNEUMATIC_CONTROLLER_H
#define PNEUMATIC_CONTROLLER_H

#include <Arduino.h>
#include "DoubleSolenoid.h"
#include "AmtEncoder.h"
#include "ButtonHandler.h"

class PneumaticController {

private:

    DoubleSolenoid* steering;
    DoubleSolenoid* resistance;
    DoubleSolenoid* switchValve;
    AmtEncoder* encoder;

    // Set via attachButtons(), checked inside waitKeepingEncoder() so
    // the START button still works even while run() is busy-waiting.
    // Null until attachButtons() is called.
    ButtonHandler* buttons;

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
    // lose encoder's quadrature counts while it's actuating. Also keeps calling
    // buttons->update() so a START press is never missed just because
    // run() is busy-waiting. Returns false (and stops all valves) if
    // START was pressed mid-wait - callers must then abort run().
    bool waitKeepingEncoder(unsigned long ms);

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

    // Call once from setup(), after both objects exist, so run()'s
    // waits can keep checking the START button.
    void attachButtons(ButtonHandler* buttons);

    void test(int command);
    void run();
    void resetRun();
};

#endif
