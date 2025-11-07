#include "halcyon.h"
#include "keyboard.h"
#include "pointing_device.h"
#include "quantum.h"
#include "rgb_matrix.h"
#include "qmk-vim/vim.h"
#include "qmk-vim/modes.h"
#include "display/game-of-life/game_of_life.h"
#include "display/trackpad-movement/trackpad-movement.h"
#include "transactions/key_pos_sync.h"
#include "transactions/trackpad_pos_sync.h"
#include "transactions/vim_mode_sync.h"
#include "transactions/trackpad_shift_sync.h"
#include "transactions/trackpad_dpi_sync.h"
#include "transactions/macro_state_sync.h"
#include "macros/macro_recorder.h"
#include "enums.h"

#define VIM_DOUBLE_J_DELAY 300

uint16_t vim_j_last_pressed = 0;
bool gaming_mode_enabled = false;

void sync_vim_mode_to_slave(void) {
    send_vim_mode_to_slave(get_vim_mode(), vim_mode_enabled());
}

void keyboard_post_init_user(void) {
    register_key_pos_sync_handler();
    register_trackpad_pos_sync_handler();
    register_vim_mode_sync_handler();
    register_trackpad_shift_sync_handler();
    register_trackpad_dpi_sync_handler();
    register_macro_state_sync_handler();

    // Initialize macro recorder
    macro_recorder_init();
}

bool module_post_init_user(void) {
    // Initialize trackpad DPI
    init_trackpad_dpi();

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

    // Process macro playback
    macro_recorder_task();

    // Ensure DPI is synced to slave after init
    trackpad_dpi_sync_task();
}

#if defined(ENCODER_MAP_ENABLE)
bool encoder_update_user(uint8_t index, bool clockwise) {
    // Record encoder movement in macro if recording
    if (macro_recorder_is_recording()) {
        macro_recorder_record_encoder(index, clockwise);
        send_macro_state_to_slave(true, macro_recorder_get_current_slot(), macro_recorder_get_recorded_count());
    }

    return true;
}
#endif

report_mouse_t pointing_device_task_user(report_mouse_t mouse_report) {
    if (mouse_report.x != 0 || mouse_report.y != 0) {
        register_trackpad_movement(mouse_report.x, mouse_report.y);
        send_trackpad_pos_to_slave(mouse_report.x, mouse_report.y);

        // Record mouse movement in macro if recording
        if (macro_recorder_is_recording()) {
            macro_recorder_record_mouse_move(mouse_report.x, mouse_report.y);
        }
    }

    return mouse_report;
}

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    // Handle macro keycodes first
    if (keycode >= MACRO_REC_1 && keycode <= MACRO_REC_10) {
        if (record->event.pressed) {
            uint8_t slot = keycode - MACRO_REC_1;
            if (macro_recorder_is_recording() && macro_recorder_get_current_slot() == slot) {
                // Stop recording if already recording this slot
                macro_recorder_stop_recording();
                send_macro_state_to_slave(false, slot, 0);
            } else if (macro_recorder_has_content(slot)) {
                // If macro exists, play it back in fast mode (no delays)
                if (macro_recorder_is_playing()) {
                    macro_recorder_stop_playback();
                }
                macro_recorder_play_fast(slot);
            } else {
                // If no macro exists, start recording
                macro_recorder_start_recording(slot);
                send_macro_state_to_slave(true, slot, 0);
            }
        }
        return false;
    }

    if (keycode >= MACRO_PLAY_1 && keycode <= MACRO_PLAY_10) {
        if (record->event.pressed) {
            uint8_t slot = keycode - MACRO_PLAY_1;
            // Stop any current playback before starting new one
            if (macro_recorder_is_playing()) {
                macro_recorder_stop_playback();
            }
            macro_recorder_play(slot);
        }
        return false;
    }

    if (keycode == MACRO_STOP) {
        if (record->event.pressed) {
            if (macro_recorder_is_recording()) {
                uint8_t slot = macro_recorder_get_current_slot();
                macro_recorder_stop_recording();
                send_macro_state_to_slave(false, slot, 0);
            }
            macro_recorder_stop_playback();
        }
        return false;
    }

    if (keycode >= MACRO_CLEAR_1 && keycode <= MACRO_CLEAR_10) {
        if (record->event.pressed) {
            uint8_t slot = keycode - MACRO_CLEAR_1;
            macro_recorder_clear(slot);
            send_macro_state_to_slave(false, slot, 0);
        }
        return false;
    }

    // Record key events for macro recording
    // Don't record macro control keys, layer switches, or transparent keys
    if (macro_recorder_is_recording() &&
        !(keycode >= MACRO_REC_1 && keycode <= MACRO_CLEAR_10) &&
        !(keycode >= QK_MOMENTARY && keycode <= QK_MOMENTARY_MAX) &&  // MO() keys
        !(keycode >= QK_LAYER_TAP && keycode <= QK_LAYER_TAP_MAX) &&  // LT() keys
        !(keycode >= QK_LAYER_MOD && keycode <= QK_LAYER_MOD_MAX) &&  // Layer mod keys
        keycode != KC_TRNS && keycode != KC_NO) {  // Transparent and no-op keys
        if (record->event.pressed) {
            macro_recorder_record_key_down(keycode, get_mods());
            // Sync action count to slave
            send_macro_state_to_slave(true, macro_recorder_get_current_slot(), macro_recorder_get_recorded_count());
        } else {
            macro_recorder_record_key_up(keycode, get_mods());
            send_macro_state_to_slave(true, macro_recorder_get_current_slot(), macro_recorder_get_recorded_count());
        }
    }

    if (!process_vim_mode(keycode, record)) {
        return false;
    }

    if (keycode == SENS_SHIFT) {
        if (record->event.pressed) {
            pointing_device_set_cpi(TRACKPAD_SHIFT_CPI);
            send_trackpad_shift_to_slave(true);
        } else {
            // Restore to saved DPI, not default
            pointing_device_set_cpi(get_synced_trackpad_dpi());
            send_trackpad_shift_to_slave(false);
        }

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
            case GAMING_TOGGLE:
                gaming_mode_enabled = !gaming_mode_enabled;
                if (gaming_mode_enabled) {
                    layer_on(_GAMING);
                    set_single_persistent_default_layer(_GAMING);
                } else {
                    layer_off(_GAMING);
                    set_single_persistent_default_layer(_QWERTY);
                }
                return false;
            case PC_LOCK:
                tap_code16(LGUI(KC_L));
                return false;
            case KC_WH_D:
                register_game_of_life_scroll(-2);
                return true;
            case KC_WH_U:
                register_game_of_life_scroll(2);
                return true;
            case KC_H:
                if ((get_mods() & MOD_MASK_CTRL) && (get_mods() & MOD_MASK_GUI)) {
                    tap_code16(LCTL(LGUI(KC_LEFT)));
                    return false;
                }
                return true;
            case KC_L:
                if ((get_mods() & MOD_MASK_CTRL) && (get_mods() & MOD_MASK_GUI)) {
                    tap_code16(LCTL(LGUI(KC_RIGHT)));
                    return false;
                }
                return true;
            case RM_RESET:
                rgb_matrix_mode(RGB_MATRIX_CUSTOM_REACTIVE_WHITE);
                return false;
            case DPI_UP:
                increase_trackpad_dpi();
                return false;
            case DPI_DOWN:
                decrease_trackpad_dpi();
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
