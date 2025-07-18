#include "halcyon.h"
#include "keyboard.h"
#include "pointing_device.h"
#include "quantum.h"
#include "display/keymap/keycode_strings.h"
#include "report.h"
#include "rgb_matrix.h"
#include "qmk-vim/vim.h"
#include "qmk-vim/modes.h"
#include "display/utils.h"
#include "display/game-of-life/game_of_life.h"
#include "display/trackpad-movement/trackpad-movement.h"
#include "transactions/key_pos_sync.h"
#include "transactions/trackpad_pos_sync.h"
#include "enums.h"

#define VIM_DOUBLE_J_DELAY 300

uint16_t vim_j_last_pressed = 0;

void keyboard_post_init_user(void) {
    register_key_pos_sync_handler();
    register_trackpad_pos_sync_handler();
    rgb_matrix_mode(RGB_MATRIX_CUSTOM_REACTIVE_WHITE);
}

bool module_post_init_user(void) {
    #ifdef HLC_CIRQUE_TRACKPAD
    pointing_device_set_cpi(300);
    #endif
    return false;
}

void pointing_device_init_user(void) {
    set_auto_mouse_layer(_MOUSE_KEYS);
    set_auto_mouse_enable(true);
}

report_mouse_t pointing_device_task_user(report_mouse_t mouse_report) {
    if (mouse_report.x != 0 || mouse_report.y != 0) {
        register_trackpad_movement(mouse_report.x, mouse_report.y);
        send_trackpad_pos_to_slave(mouse_report.x, mouse_report.y);
    }

    return mouse_report;
}

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    if (!process_vim_mode(keycode, record)) {
        return false;
    }

    if (record->event.pressed) {
        register_game_of_life_key_press(record->event.key.row, record->event.key.col);
        send_key_pos_to_slave(record->event.key.row, record->event.key.col);
        // return false;
    }

    switch (keycode) {
        case VIM_TOGGLE:
                uprintf("toggle_vim_mode pressed\n");
            if (record->event.pressed) {
                uprintf("toggle_vim_mode\n");
                toggle_vim_mode();
            }
            return false;
    }

    if (record->event.pressed) {
        // uprintf("key pressed: %u (%s)\n", keycode, get_keycode_string_hlc(keycode));
        // uprintf("row: %d, col: %d\n", record->event.key.row, record->event.key.col);
    }

    return true;
}

bool process_insert_mode_user(uint16_t keycode, keyrecord_t *record) {
    if (record->event.pressed) {
        if (keycode == KC_J) {
            uint16_t now = timer_read();
            if (now - vim_j_last_pressed < VIM_DOUBLE_J_DELAY) {
                tap_code(KC_BSPC);
                normal_mode();
                vim_j_last_pressed = 0;
                return false;
            }

            vim_j_last_pressed = now;
        } else {
            vim_j_last_pressed = 0;
        }

        if (keycode == LCTL(KC_W)) {
            tap_code16(LCTL(KC_BSPC));
            return false;
        }
    }

    return true;
}
