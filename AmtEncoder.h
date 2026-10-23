
#ifndef AMT_ENCODER_H
#define AMT_ENCODER_H

#include <Arduino.h>
#include <stdint.h>

// One AMT103 quadrature encoder, read through Opta analog inputs.
class AmtEncoder {

private:
    const char* name;
    int pinA;
    int pinB;
    int ppr;

    long count;
    uint8_t lastState;
    bool homed;

    int levelA;
    int levelB;

    bool stateA;
    bool stateB;

    bool readWithHysteresis(int pin, bool currentState, int &outLevel);

public:
    AmtEncoder(const char* name, int pinA, int pinB, int ppr);

    void begin();
    void update();
    void zero();

    float angleDeg() const;

    bool isHomed() const;

    int rawA() const;
    int rawB() const;
};

#endif
