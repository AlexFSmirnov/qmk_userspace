#include "halcyon.h"
#include "keyboard.h"
#include "pointing_device.h"
#include "quantum.h"
#include "rgb_matrix.h"
#include "qmk-vim/vim.h"
#include "qmk-vim/modes.h"
#include "display/game-of-life/game_of_life.h"
#include "display/trackpad-movement/trackpad-movement.h"
#include "display/controller.h"
#include "transactions/key_pos_sync.h"
#include "transactions/trackpad_pos_sync.h"
#include "transactions/vim_mode_sync.h"
#include "enums.h"

#define VIM_DOUBLE_J_DELAY 300

uint16_t vim_j_last_pressed = 0;

void sync_vim_mode_to_slave(void) {
    send_vim_mode_to_slave(get_vim_mode(), vim_mode_enabled());
}

void keyboard_post_init_user(void) {
    register_key_pos_sync_handler();
    register_trackpad_pos_sync_handler();
    register_vim_mode_sync_handler();
}

bool module_post_init_user(void) {
    #ifdef HLC_CIRQUE_TRACKPAD
    pointing_device_set_cpi(300);
    #endif

    #ifdef HLC_TFT_DISPLAY
    init_display_surfaces();
    #endif

    sync_vim_mode_to_slave();

    return false;
}

void pointing_device_init_user(void) {
    set_auto_mouse_layer(_MOUSE_KEYS);
    set_auto_mouse_enable(true);
}

void housekeeping_task_user(void) {
    if (is_synced_vim_mode_outdated()) {
        sync_vim_mode_to_slave();
    }
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

        switch (keycode) {
            case VIM_TOGGLE:
                toggle_vim_mode();
                sync_vim_mode_to_slave();
                return false;
            case PC_LOCK:
                tap_code16(LGUI(KC_L));
                return false;
            case KC_H:
                if ((get_mods() & MOD_MASK_CTRL) && (get_mods() & MOD_MASK_GUI)) {
                    tap_code16(LCTL(LGUI(KC_LEFT)));
                    return false;
                }
                return false;
            case KC_L:
                if ((get_mods() & MOD_MASK_CTRL) && (get_mods() & MOD_MASK_GUI)) {
                    tap_code16(LCTL(LGUI(KC_RIGHT)));
                    return false;
                }
                return false;
            case RM_RESET:
                rgb_matrix_mode(RGB_MATRIX_CUSTOM_REACTIVE_WHITE);
                return false;
        }
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
                sync_vim_mode_to_slave();
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
