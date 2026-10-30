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
        constexpr int START_BUTTON  = A0;  // I1, L1 Opta

        // AMT103, read as 0-10 V analog. X = index, once per turn.
        constexpr int ENC1_A        = A2;  // I3
        constexpr int ENC1_B        = A3;  // I4
        constexpr int ENC1_X        = A4;  // I5
        constexpr int ENC2_A        = A5;  // I6
        constexpr int ENC2_B        = A6;  // I7
        constexpr int ENC2_X        = A7;  // I8
    }

    namespace Encoder {

        // Must match the DIP switch on the AMT103.
        constexpr int PPR = 2048;

        // Both encoders are wired.
        constexpr bool USE_ENCODER2 = true;

        // 12-bit reading: low is about 0, high is about 1400.
        // Hysteresis band so noise/vibration near the switching point
        // (e.g. cylinder hitting its end-stop) can't flip the decoded
        // state back and forth and create phantom counts.
        constexpr int ADC_BITS = 12;
        constexpr int HIGH_THRESHOLD = 700;  // go HIGH above this
        constexpr int LOW_THRESHOLD  = 200;  // go LOW below this

        // Live angle print period.
        constexpr unsigned long PRINT_MS = 50;
    }

    namespace Timing {

        constexpr unsigned long DEBOUNCE_MS          = 30;
        constexpr unsigned long USER_CLICK_WINDOW_MS = 3000;
        constexpr unsigned long START_LED_BLINK_MS   = 500;

        // Rocker Switch valve pulse (ON/OFF), then both coils OFF
        constexpr unsigned long SWITCH_PULSE_MS   = 500;

        // Pause with steering + resistance released (stopped) before
        // every switch pulse (left or right).
        constexpr unsigned long RELEASE_HOLD_MS   = 1000;

        // How long one steering stroke needs to physically finish
        // before the next valve command is issued. Tune this to match
        // the real cylinder travel time on the rig.
        constexpr unsigned long STEER_STROKE_MS   = 1000;
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
