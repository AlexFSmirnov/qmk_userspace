#include "qp.h"
#include "quantum.h"
#include "hlc_tft_display/hlc_tft_display.h"
#include "qp_surface.h"
#include "controller.h"
#include "keymap/process_keymap.h"
#include "qmk-vim/vim.h"
#include "game-of-life/game_of_life.h"
#include "utils.h"

#define LCD_WIDTH 135
#define LCD_HEIGHT 240

extern painter_device_t lcd;
extern painter_device_t lcd_surface;

// External variables from user.c for key position tracking
extern int current_key_row;
extern int current_key_col;
extern bool key_pressed_for_display;

bool module_post_init_user(void) {
    // Draw the keymap layout using the new module
    draw_keymap_layout(lcd_surface);
    qp_surface_draw(lcd_surface, lcd, 0, 0, 0);

    return false;
}

bool display_module_housekeeping_task_user(bool second_display) {
    bool should_redraw = false;

    uint8_t active_layer = get_highest_layer(layer_state | default_layer_state);

    if (active_layer == 0) {
        if (process_game_of_life_display(lcd_surface)) {
            should_redraw = true;
        }
    } else {
        reset_game_of_life_grid();
    }

    // if (process_keymap_display(lcd_surface)) {
    //     should_redraw = true;
    // }

    if (vim_mode_enabled()) {
        qp_rect(lcd_surface, 50, 50, 100, 100, HSV_RED, 1);
        should_redraw = true;
    }

    if (should_redraw) {
        qp_surface_draw(lcd_surface, lcd, 0, 0, 0);
    }

    return false;
}
