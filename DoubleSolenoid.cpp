#include "DoubleSolenoid.h"

// =====================================================
// CONSTRUCTOR CALL..
// =====================================================
DoubleSolenoid::DoubleSolenoid(
    DigitalStSolidExpansion* exp,
    int s1,
    int s2
) {
    this->exp = exp;
    this->s1 = s1;
    this->s2 = s2;
}

// =====================================================
// BEGIN Method
// =====================================================
void DoubleSolenoid::begin() {

    // Both solenoids OFF as ground state
    exp->digitalWrite(s1, LOW);
    exp->digitalWrite(s2, LOW);
    exp->updateDigitalOutputs();
}

// =====================================================
// EXTEND Method
// =====================================================
void DoubleSolenoid::extend() {

    // Solenoid 2 OFF
    exp->digitalWrite(s2, LOW);
    // Solenoid 1 ON
    exp->digitalWrite(s1, HIGH);
    exp->updateDigitalOutputs();
}

// =====================================================
// RETRACT Method
// =====================================================
void DoubleSolenoid::retract() {

    // Solenoid 1 OFF
    exp->digitalWrite(s1, LOW);
    // Solenoid 2 ON
    exp->digitalWrite(s2, HIGH);
    exp->updateDigitalOutputs();
}

// =====================================================
// STOP Method
// =====================================================
void DoubleSolenoid::stop() {

    // Both solenoids OFF
    exp->digitalWrite(s1, LOW);
    exp->digitalWrite(s2, LOW);
    exp->updateDigitalOutputs();
}
