#include "keycode_strings.h"

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

        // Common modifier keys
        case KC_LCTL: return "CTL";
        case KC_RCTL: return "CTL";
        case KC_LSFT: return "SFT";
        case KC_RSFT: return "SFT";
        case KC_LALT: return "ALT";
        case KC_RALT: return "ALT";
        case KC_LGUI: return "GUI";
        case KC_RGUI: return "GUI";

        // Common keys
        case KC_TAB: return "TAB";
        case KC_ESC: return "ESC";
        case KC_ENT: return "ENT";
        case KC_BSPC: return "BSP";
        case KC_SPC: return "SPC";
        case KC_DEL: return "DEL";
        case KC_INS: return "INS";
        case KC_HOME: return "HOME";
        case KC_END: return "END";
        case KC_PGUP: return "PGUP";
        case KC_PGDN: return "PGDN";

        // Arrow keys
        case KC_LEFT: return "←";
        case KC_RGHT: return "→";
        case KC_UP: return "↑";
        case KC_DOWN: return "↓";

        // Punctuation and symbols
        case KC_COMM: return ",";
        case KC_DOT: return ".";
        case KC_SLSH: return "/";
        case KC_SCLN: return ";";
        case KC_QUOT: return "'";
        case KC_GRV: return "`";
        case KC_TILD: return "~";
        case KC_EXLM: return "!";
        case KC_AT: return "@";
        case KC_HASH: return "#";
        case KC_DLR: return "$";
        case KC_PERC: return "%";
        case KC_CIRC: return "^";
        case KC_AMPR: return "&";
        case KC_ASTR: return "*";
        case KC_LPRN: return "(";
        case KC_RPRN: return ")";
        case KC_UNDS: return "_";
        case KC_PLUS: return "+";
        case KC_EQL: return "=";
        case KC_MINS: return "-";
        case KC_LBRC: return "[";
        case KC_RBRC: return "]";
        case KC_LCBR: return "{";
        case KC_RCBR: return "}";
        case KC_PIPE: return "|";
        case KC_BSLS: return "\\";
        case KC_COLN: return ":";
        case KC_QUES: return "?";

        // Media keys
        case KC_MUTE: return "MUTE";
        case KC_VOLU: return "VOL+";
        case KC_VOLD: return "VOL-";
        case KC_MPLY: return "PLAY";
        case KC_MSTP: return "STOP";
        case KC_MPRV: return "PREV";
        case KC_MNXT: return "NEXT";

        // Layer keys
        case KC_TRNS: return "";
        case KC_NO: return "";

        default: return "-";
    }
}
