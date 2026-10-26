#include "PneumaticController.h"
#include "Config.h"

// mode 1 (TRADITIONAL): switch LEFT, steer left/right 3 times, then mode 2.
// mode 2 (STEERBYLEAN): switch RIGHT, steer left/right 3 times, then mode 1.
//
// Uses waitKeepingEncoder() instead of delay(): same blocking wait, but it
// keeps sampling encoder->update() the whole time, so a fast stroke can't
// lose quadrature counts just because run() is busy-waiting.
void PneumaticController::run() {

    int count = 3;
    int steercount;

    switch(mode) {

        // mode = 1 --> TRADITIONAL (default in constructor of the pneumatic object, check PneumaticController.cpp)
        case 1:

            steercount = count;
            switchValve -> extend();
            waitKeepingEncoder(Config::Timing::SWITCH_PULSE_MS);
            switchValve -> stop();

            while(steercount != 0) {
                // steering left
                steering -> retract();
                // resistance right
                resistance -> extend();
                waitKeepingEncoder(Config::Timing::STEER_STROKE_MS);
                steering -> extend();
                resistance -> retract();
                waitKeepingEncoder(Config::Timing::STEER_STROKE_MS);

                steercount --;
            }
            steercount = count;
            mode = 2;

            break;


        // mode = 2 --> STEERBYLEAN :)
        case 2:

            steercount = count;
            switchValve -> retract();
            waitKeepingEncoder(Config::Timing::SWITCH_PULSE_MS);
            switchValve -> stop();

            while(steercount != 0){
                // steering right
                steering -> extend();
                //resistance left
                resistance -> retract();
                waitKeepingEncoder(Config::Timing::STEER_STROKE_MS);
                steering -> retract();
                resistance -> extend();
                waitKeepingEncoder(Config::Timing::STEER_STROKE_MS);

                steercount --;
            }

            steercount = count;
            mode = 1;

            break;

    }
}

void PneumaticController::waitKeepingEncoder(unsigned long ms) {
    unsigned long start = millis();

    while (millis() - start < ms) {
        encoder->update();
    }
}
