#ifndef BUTTON_HANDLER_H
#define BUTTON_HANDLER_H

#include <Arduino.h>
#include "OptaBlue.h"
#include "Config.h"

using namespace Opta;

class ButtonHandler {

private:

    // USER BUTTON
    int userPin;
    bool lastUserState;
    unsigned long userDebounceTime;
    unsigned long firstClickTime;
    int clickCount;

    // START BUTTON
    int startPin;
    bool lastStartState;
    unsigned long startDebounceTime;

    // CALIBRATION SENSOR
    int calibrationSensorPin;

    // START LED
    DigitalStSolidExpansion* exp;

    unsigned long ledBlinkTime;
    bool ledState;

    // SYSTEM STATE
    enum SystemMode {
        IDLE,
        CALIBRATION,
        RUNNING
    };

    SystemMode systemMode;

    // EVENTS
    int userCommand;
    bool startEvent;

    void handleUserButton();
    void handleStartButton();
    void handleStartLed();

    void ledOn();
    void ledOff();

public:

    ButtonHandler(
        int userPin,
        int startPin,
        DigitalStSolidExpansion* expansion,
        int calibrationSensorPin
    );

    void begin();
    void update();

    int getUserCommand();
    bool startPressed();

    bool isCalibrationComplete();
    bool isRunning();
    bool isIdle();
};

#endif
