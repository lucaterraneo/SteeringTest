#include "ButtonHandler.h"

// =====================================================
// CONSTRUCTOR
// =====================================================
ButtonHandler::ButtonHandler(
    int userPin,
    int startPin,
    DigitalStSolidExpansion* expansion,
    int calibrationSensorPin
) {
    this->userPin = userPin;
    this->startPin = startPin;
    this->calibrationSensorPin = calibrationSensorPin;

    exp = expansion;

    // USER BUTTON
    lastUserState = HIGH;
    userDebounceTime = 0;

    firstClickTime = 0;
    clickCount = 0;

    // START BUTTON
    lastStartState = LOW;
    startDebounceTime = 0;

    // EVENTS
    userCommand = 0;
    startEvent = false;

    // START LED
    ledBlinkTime = 0;
    ledState = false;

    // Initial system state
    systemMode = IDLE;
}

// =====================================================
// BEGIN
// =====================================================
void ButtonHandler::begin() {

    pinMode(userPin, INPUT_PULLUP);

    // START button receives 24 V when pressed
    pinMode(startPin, INPUT);

    // Calibration sensor
    pinMode(calibrationSensorPin, INPUT);

    // Read initial states
    lastUserState = digitalRead(userPin);
    lastStartState = digitalRead(startPin);

    userCommand = 0;
    startEvent = false;

    clickCount = 0;

    // Start in IDLE
    systemMode = IDLE;

    // START LED OFF
    ledOff();

    Serial.println("ButtonHandler initialized");
    Serial.println("SYSTEM -> IDLE");
}

// =====================================================
// UPDATE
// =====================================================
void ButtonHandler::update() {

    handleUserButton();
    handleStartButton();
    handleStartLed();
}

// =====================================================
// USER BUTTON
// =====================================================
void ButtonHandler::handleUserButton() {

    bool buttonState = digitalRead(userPin);
    unsigned long now = millis();

    // Detect USER button press
    if (buttonState == LOW && lastUserState == HIGH) {

        if (now - userDebounceTime >= Config::Timing::DEBOUNCE_MS) {

            userDebounceTime = now;

            // First click starts the timer
            if (clickCount == 0) {
                firstClickTime = now;
                Serial.println("\nUSER click sequence started...");
            }

            // Count click
            clickCount++;

            Serial.print("USER click detected: ");
            Serial.println(clickCount);
        }
    }

    lastUserState = buttonState;

    // Evaluate click sequence
    if (clickCount > 0) {

        if (now - firstClickTime >= Config::Timing::USER_CLICK_WINDOW_MS) {

            Serial.print("USER command ready: ");
            Serial.println(clickCount);

            // Store command
            userCommand = clickCount;

            // Reset click counter
            clickCount = 0;
        }
    }
}

// =====================================================
// START BUTTON
// =====================================================
void ButtonHandler::handleStartButton() {

    bool buttonState = digitalRead(startPin);
    unsigned long now = millis();

    // Detect START button press
    if (buttonState == HIGH && lastStartState == LOW) {

        if (now - startDebounceTime >= Config::Timing::DEBOUNCE_MS) {

            startDebounceTime = now;

            startEvent = true;

            // -----------------------------------------
            // IDLE -> CALIBRATION
            // -----------------------------------------
            if (systemMode == IDLE) {

                systemMode = CALIBRATION;

                ledBlinkTime = now;
                ledState = false;

                ledOff();

                Serial.println("SYSTEM -> CALIBRATION");
            }

            // -----------------------------------------
            // CALIBRATION -> RUNNING
            // -----------------------------------------
            else if (systemMode == CALIBRATION) {

                // TEMPORARY... Fake the calibration sensor signal.
                // This simulates A1 becoming HIGH.
                bool calibrationReached = true;

                /*bool calibrationReached =
                    digitalRead(calibrationSensorPin) == HIGH;
                */

                if (calibrationReached) {

                    systemMode = RUNNING;

                    ledOn();

                    Serial.println("FAKE A1 SENSOR -> ON");
                    Serial.println("SYSTEM -> RUNNING");
                }
                else {

                    Serial.println(
                        "START ignored: calibration position not reached"
                    );
                }
            }

            // -----------------------------------------
            // RUNNING -> IDLE
            // -----------------------------------------
            else if (systemMode == RUNNING) {

                systemMode = IDLE;

                ledOff();

                Serial.println("SYSTEM -> IDLE");
            }
        }
    }

    lastStartState = buttonState;
}

// =====================================================
// START LED HANDLER
// =====================================================
void ButtonHandler::handleStartLed() {

    // LED must blink only during calibration
    if (systemMode != CALIBRATION) {
        return;
    }

    unsigned long now = millis();

    if (now - ledBlinkTime >= Config::Timing::START_LED_BLINK_MS) {

        ledBlinkTime = now;

        ledState = !ledState;

        if (ledState) {
            ledOn();
        }
        else {
            ledOff();
        }
    }
}

// =====================================================
// LED ON
// =====================================================
void ButtonHandler::ledOn() {

    // Physical SSR7 = software index 6
    exp->digitalWrite(Config::Pins::START_LED, HIGH);
    exp->updateDigitalOutputs();

    ledState = true;
}

// =====================================================
// LED OFF
// =====================================================
void ButtonHandler::ledOff() {

    // Physical SSR7 = software index 6
    exp->digitalWrite(Config::Pins::START_LED, LOW);
    exp->updateDigitalOutputs();

    ledState = false;
}

// =====================================================
// GET USER COMMAND
// =====================================================
int ButtonHandler::getUserCommand() {

    int command = userCommand;
    userCommand = 0;

    return command;
}

// =====================================================
// START PRESSED
// =====================================================
bool ButtonHandler::startPressed() {

    bool event = startEvent;
    startEvent = false;

    return event;
}

// =====================================================
// SYSTEM STATE
// =====================================================
bool ButtonHandler::isCalibrationComplete() {

    return digitalRead(calibrationSensorPin) == HIGH;
}

bool ButtonHandler::isRunning() {

    return systemMode == RUNNING;
}

bool ButtonHandler::isIdle() {

    return systemMode == IDLE;
}
