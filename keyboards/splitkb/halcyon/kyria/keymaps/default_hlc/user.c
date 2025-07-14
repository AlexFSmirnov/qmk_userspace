#include "quantum.h"
#include "display/keymap/keycode_strings.h"

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    if (record->event.pressed) {
        uprintf("Key pressed: %u (%s)\n", keycode, get_keycode_string_hlc(keycode));
    }
    return true;
}
