#include "PneumaticController.h"
#include "Config.h"

// mode 1 (TRADITIONAL): switch LEFT, steer left/right 3 times, then mode 2.
// mode 2 (STEERBYLEAN): switch RIGHT, steer left/right 3 times, then mode 1.
//
// Uses waitKeepingEncoder() instead of delay(): same blocking wait, but it
// keeps sampling encoder->update() the whole time, so a fast stroke can't
// lose quadrature counts (angle) just because run() is busy-waiting.
void PneumaticController::run() {

    int count = 3;
    int steercount;

    switch(mode) {

        // mode = 1 --> TRADITIONAL (default in constructor of the pneumatic object, check PneumaticController.cpp)
        case 1:

            steercount = count;

            // release pneumatics to allow for a representative situation and switch
            resistance -> stop();
            steering -> stop();
            if (!waitKeepingEncoder(Config::Timing::STEER_STROKE_MS)) return;

            switchValve -> extend();
            if (!waitKeepingEncoder(Config::Timing::SWITCH_PULSE_MS)) return;
            switchValve -> stop();

            // release pneumatics just after switch has been actuated
            resistance -> stop();
            steering -> stop();
            if (!waitKeepingEncoder(Config::Timing::STEER_STROKE_MS)) return;


            while(steercount != 0) {
                // steering left
                steering -> retract();
                // resistance right
                resistance -> extend();
                if (!waitKeepingEncoder(Config::Timing::STEER_STROKE_MS)) return;
                steering -> extend();
                resistance -> retract();
                if (!waitKeepingEncoder(Config::Timing::STEER_STROKE_MS)) return;

                steercount --;
            }
            steercount = count;
            mode = 2;

            break;


        // mode = 2 --> STEERBYLEAN :)
        case 2:

            steercount = count;

            // release pneumatics to allow for a representative situation and switch
            resistance -> stop();
            steering -> stop();
            if (!waitKeepingEncoder(Config::Timing::STEER_STROKE_MS)) return;

            switchValve -> retract();
            if (!waitKeepingEncoder(Config::Timing::SWITCH_PULSE_MS)) return;
            switchValve -> stop();

            // release pneumatics just after switch has been actuated
            resistance -> stop();
            steering -> stop();
            if (!waitKeepingEncoder(Config::Timing::STEER_STROKE_MS)) return;


            while(steercount != 0){
                // steering right
                steering -> extend();
                //resistance left
                resistance -> retract();
                if (!waitKeepingEncoder(Config::Timing::STEER_STROKE_MS)) return;
                steering -> retract();
                resistance -> extend();
                if (!waitKeepingEncoder(Config::Timing::STEER_STROKE_MS)) return;

                steercount --;
            }
            steercount = count;
            mode = 1;

            break;

    }
}

bool PneumaticController::waitKeepingEncoder(unsigned long ms) {
    unsigned long start = millis();
    unsigned long lastPrint = start;

    while (millis() - start < ms) {
        encoder->update();

        // Keep the START button alive even while run() is busy-waiting,
        // so pressing it stops the whole sequence right away instead
        // of being missed until run() eventually returns.
        if (buttons) {
            buttons->update();

            if (!buttons->isRunning()) {
                stopAll();
                return false;
            }
        }

        unsigned long now = millis();
        if (now - lastPrint >= Config::Encoder::PRINT_MS) {
            lastPrint = now;
            Serial.print("E1 | Angle: ");
            Serial.println(encoder->angleDeg(), 2);
        }
    }

    return true;
}
