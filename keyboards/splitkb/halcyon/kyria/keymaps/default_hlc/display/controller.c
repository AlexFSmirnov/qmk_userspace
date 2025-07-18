#include "display/trackpad-movement/trackpad-movement.h"
#include "qp.h"
#include "quantum.h"
#include "hlc_tft_display/hlc_tft_display.h"
#include "qp_surface.h"
#include "controller.h"
#include "keymap/process_keymap.h"
#include "qmk-vim/vim.h"
#include "game-of-life/game_of_life.h"
#include "utils.h"
#include "enums.h"

#define LCD_WIDTH 135
#define LCD_HEIGHT 240

painter_device_t lcd;
painter_device_t lcd_surface;

// External variables from user.c for key position tracking
extern int current_key_row;
extern int current_key_col;
extern bool key_pressed_for_display;

bool display_module_housekeeping_task_user(bool second_display) {
    #ifndef HLC_TFT_DISPLAY
    return false;
    #endif

    bool should_redraw = false;

    uint8_t active_layer = get_highest_layer(layer_state | default_layer_state);

    if (active_layer == 0) {
        should_redraw = process_game_of_life_display(lcd_surface) || should_redraw;
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

    if (should_redraw) {
        qp_surface_draw(lcd_surface, lcd, 0, 0, false);
    }

    return false;
}
