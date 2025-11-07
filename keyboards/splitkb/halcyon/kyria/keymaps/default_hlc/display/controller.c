#include "display/trackpad-movement/trackpad-movement.h"
#include "qp.h"
#include "quantum.h"
#include "hlc_tft_display/hlc_tft_display.h"
#include "qp_surface.h"
#include "controller.h"
#include "keymap/process_keymap.h"
#include "qmk-vim/vim.h"
#include "vim-header/vim-header.h"
#include "../transactions/vim_mode_sync.h"
#include "../transactions/macro_state_sync.h"
#include "game-of-life/game_of_life.h"
#include "utils.h"
#include "notification/notification.h"
#include "macro-status/macro-status.h"
#include "enums.h"

#define LCD_WIDTH 135
#define LCD_HEIGHT 240

painter_device_t lcd;
painter_device_t lcd_surface;

// External variables from user.c for key position tracking
extern int current_key_row;
extern int current_key_col;
extern bool key_pressed_for_display;

bool was_vim_mode_enabled = false;

bool display_module_housekeeping_task_user(bool second_display) {
    #ifndef HLC_TFT_DISPLAY
    return false;
    #endif

    bool should_redraw = false;

    uint8_t active_layer = get_highest_layer(layer_state | default_layer_state);

    if (active_layer == _QWERTY || active_layer == _GAMING) {
        should_redraw = process_game_of_life_display(lcd_surface) || should_redraw;
        if (should_redraw) {
            clear_vim_header();
        }
    }

    if (active_layer == _MOUSE_KEYS) {
        should_redraw = process_trackpad_movement_display(lcd_surface) || should_redraw;
    } else {
        clear_trackpad_movement();
    }

    if (active_layer != 0 && active_layer != _MOUSE_KEYS) {
        should_redraw = process_keymap_display(lcd_surface) || should_redraw;
    } else {
        reset_keymap_display();
    }

    if (get_synced_vim_enabled() && active_layer != _MOUSE_KEYS) {
        should_redraw = process_vim_header_display(lcd_surface) || should_redraw;
        was_vim_mode_enabled = true;
    } else {
        clear_vim_header();
        if (was_vim_mode_enabled) {
            reset_keymap_display();
            was_vim_mode_enabled = false;
        }
    }

    // Process notification overlay (always check, as it might need to clear itself)
    should_redraw = process_notification_display(lcd_surface) || should_redraw;

    // Process macro status display (shows recording status)
    // Check on slave side if master is recording
    #ifdef SPLIT_KEYBOARD
    if (!is_keyboard_master()) {
        if (get_synced_macro_recording_state()) {
            should_redraw = true;
        }
    }
    #endif
    should_redraw = process_macro_status_display(lcd_surface) || should_redraw;

    if (should_redraw) {
        qp_surface_draw(lcd_surface, lcd, 0, 0, false);
    }

    return false;
}
