#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>

// ============================================================
//  Change the parameters here. No need to touch the other files.
//  SSR indices: 0 = first relay of the expansion, 6 = SSR 7.
// ============================================================

namespace Config {

    namespace Pins {

        // Valves (SSR pair: A = extend, B = retract)
        constexpr int STEERING_A    = 0;
        constexpr int STEERING_B    = 1;
        constexpr int RESISTANCE_A  = 2;
        constexpr int RESISTANCE_B  = 3;
        constexpr int SWITCH_A      = 4;
        constexpr int SWITCH_B      = 5;

        constexpr int START_LED     = 6;   // SSR 7

        constexpr int USER_BUTTON   = BTN_USER;
        constexpr int START_BUTTON  = A0;  // L1 Opta
        constexpr int CALIBRATION   = A1;
    }

    namespace Timing {

        constexpr unsigned long DEBOUNCE_MS          = 30;
        constexpr unsigned long USER_CLICK_WINDOW_MS = 3000;
        constexpr unsigned long START_LED_BLINK_MS   = 500;

        // Switch valve pulse (ON/OFF), then both coils OFF
        constexpr unsigned long SWITCH_PULSE_MS   = 500;
    }

    namespace SerialPort {
        constexpr unsigned long BAUD = 115200;
    }

    // Clicks on the USER button within USER_CLICK_WINDOW_MS
    enum TestCommand {
        CMD_STOP             = 0,
        CMD_STEERING_RIGHT   = 1,
        CMD_STEERING_LEFT    = 2,
        CMD_RESISTANCE_LEFT  = 3,
        CMD_RESISTANCE_RIGHT = 4,
        CMD_SWITCH_ON        = 5,
        CMD_SWITCH_OFF       = 6
    };

}

#endif
