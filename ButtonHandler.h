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
        DigitalStSolidExpansion* expansion
    );

    void begin();
    void update();

    int getUserCommand();
    bool startPressed();

    bool isRunning();
    bool isIdle();
    bool isCalibrating();
};

#endif
