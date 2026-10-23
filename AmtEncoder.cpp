#include "AmtEncoder.h"
#include "Config.h"

static const int8_t QUAD_STEP[16] = {
     0, -1, +1,  0,
    +1,  0,  0, -1,
    -1,  0,  0, +1,
     0, +1, -1,  0
};

AmtEncoder::AmtEncoder(const char* name, int pinA, int pinB, int ppr) {
    this->name = name;
    this->pinA = pinA;
    this->pinB = pinB;
    this->ppr = ppr;

    count = 0;
    lastState = 0;
    homed = false;

    levelA = 0;
    levelB = 0;

    stateA = false;
    stateB = false;
}

bool AmtEncoder::readWithHysteresis(
    int pin,
    bool currentState,
    int &outLevel
) {
    outLevel = analogRead(pin);

    if (currentState) {
        return outLevel >= Config::Encoder::LOW_THRESHOLD;
    }

    return outLevel > Config::Encoder::HIGH_THRESHOLD;
}

void AmtEncoder::begin() {
    analogReadResolution(Config::Encoder::ADC_BITS);

    bool a = readWithHysteresis(pinA, stateA, levelA);
    bool b = readWithHysteresis(pinB, stateB, levelB);

    stateA = a;
    stateB = b;

    lastState = (a ? 2 : 0) | (b ? 1 : 0);

    count = 0;
    homed = false;
}

void AmtEncoder::update() {
    // Read both encoder channels.
    bool a = readWithHysteresis(pinA, stateA, levelA);
    bool b = readWithHysteresis(pinB, stateB, levelB);

    stateA = a;
    stateB = b;

    uint8_t state = (a ? 2 : 0) | (b ? 1 : 0);

    // Quadrature decoding.
    uint8_t transition = (lastState << 2) | state;
    count += QUAD_STEP[transition];

    lastState = state;
}

void AmtEncoder::zero() {
    count = 0;
    homed = true;

    Serial.print(name);
    Serial.println(" zero");
}

float AmtEncoder::angleDeg() const {
    if (ppr <= 0) {
        return 0.0f;
    }

    return ((float)count * 360.0f) / ((float)ppr * 4.0f);
}

bool AmtEncoder::isHomed() const {
    return homed;
}

int AmtEncoder::rawA() const {
    return levelA;
}

int AmtEncoder::rawB() const {
    return levelB;
}
