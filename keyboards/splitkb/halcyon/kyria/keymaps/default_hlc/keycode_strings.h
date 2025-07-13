// Copyright 2024 splitkb.com (support@splitkb.com)
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "quantum.h"

// Helper function to get string representation of a keycode
const char* get_keycode_string_hlc(uint16_t keycode);

// Function implementation
const char* get_keycode_string_hlc(uint16_t keycode) {
    switch (keycode) {
        // KC_A through KC_Z
        case KC_A: return "A";
        case KC_B: return "B";
        case KC_C: return "C";
        case KC_D: return "D";
        case KC_E: return "E";
        case KC_F: return "F";
        case KC_G: return "G";
        case KC_H: return "H";
        case KC_I: return "I";
        case KC_J: return "J";
        case KC_K: return "K";
        case KC_L: return "L";
        case KC_M: return "M";
        case KC_N: return "N";
        case KC_O: return "O";
        case KC_P: return "P";
        case KC_Q: return "Q";
        case KC_R: return "R";
        case KC_S: return "S";
        case KC_T: return "T";
        case KC_U: return "U";
        case KC_V: return "V";
        case KC_W: return "W";
        case KC_X: return "X";
        case KC_Y: return "Y";
        case KC_Z: return "Z";

        // KC_0 through KC_9
        case KC_0: return "0";
        case KC_1: return "1";
        case KC_2: return "2";
        case KC_3: return "3";
        case KC_4: return "4";
        case KC_5: return "5";
        case KC_6: return "6";
        case KC_7: return "7";
        case KC_8: return "8";
        case KC_9: return "9";

        // KC_F1 through KC_F20
        case KC_F1: return "F1";
        case KC_F2: return "F2";
        case KC_F3: return "F3";
        case KC_F4: return "F4";
        case KC_F5: return "F5";
        case KC_F6: return "F6";
        case KC_F7: return "F7";
        case KC_F8: return "F8";
        case KC_F9: return "F9";
        case KC_F10: return "F10";
        case KC_F11: return "F11";
        case KC_F12: return "F12";
        case KC_F13: return "F13";
        case KC_F14: return "F14";
        case KC_F15: return "F15";
        case KC_F16: return "F16";
        case KC_F17: return "F17";
        case KC_F18: return "F18";
        case KC_F19: return "F19";
        case KC_F20: return "F20";

        default: return "UNKNOWN";
    }
}
