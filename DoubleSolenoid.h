#ifndef DOUBLE_SOLENOID_H
#define DOUBLE_SOLENOID_H

#include <Arduino.h>
#include "OptaBlue.h"

using namespace Opta;

class DoubleSolenoid {

private:

    DigitalStSolidExpansion* exp;
    int s1;
    int s2;

public:

    DoubleSolenoid(
        DigitalStSolidExpansion* exp,
        int s1,
        int s2
    );

    void begin();
    void extend();
    void retract();
    void stop();
};

#endif
