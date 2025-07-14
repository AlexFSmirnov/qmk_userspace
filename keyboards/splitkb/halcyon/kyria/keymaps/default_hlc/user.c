#include "keyboard.h"
#include "quantum.h"
#include "display/keymap/keycode_strings.h"
#include "rgb_matrix.h"

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    if (record->event.pressed) {
        uprintf("Key pressed: %u (%s)\n", keycode, get_keycode_string_hlc(keycode));
    }
    if (record->event.pressed) {
        switch (keycode) {
            case KC_P:
                if (record->event.pressed) {
                    rgb_matrix_mode(RGB_MATRIX_RAINBOW_MOVING_CHEVRON);
                }
                return false;
            case KC_SCLN:
                if (record->event.pressed) {
                    // rgb_matrix_mode_noeeprom(RGB_MATRIX_CUSTOM_CUSTOM_BREATHING);
                    rgb_matrix_mode(RGB_MATRIX_CUSTOM_REACTIVE_WHITE);
                }
                return false;
            case KC_SLSH:
                if (record->event.pressed) {
                    rgb_matrix_mode(RGB_MATRIX_CUSTOM_REACTIVE_WHITE);
                }
                return false;
        }
    }
    return true;
}
