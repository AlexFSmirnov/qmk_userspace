#include "qp.h"
#include "quantum.h"
#include "keycode_strings.h"
#include "hlc_tft_display/hlc_tft_display.h"
#include "qp_surface.h"
#include "display/draw_layout.h"
#include "display/keymap_display.h"

#define LCD_WIDTH 135
#define LCD_HEIGHT 240

painter_device_t lcd;
painter_device_t lcd_surface;

bool module_post_init_user(void) {
    // Draw the keymap layout using the new module
    draw_keymap_layout(lcd_surface);
    qp_surface_draw(lcd_surface, lcd, 0, 0, 0);

    return false;
}

bool display_module_housekeeping_task_user(bool second_display) {
    static layer_state_t last_layer_state = 0;
    static uint8_t last_default_layer = 0;

    // Check if we need to redraw the keymap
    if (should_redraw_keymap(last_layer_state, last_default_layer)) {
        // Redraw the layout grid with the updated keymap
        draw_keymap_layout(lcd_surface);
        qp_surface_draw(lcd_surface, lcd, 0, 0, 0);

        // Update our stored states
        update_layer_states(&last_layer_state, &last_default_layer);
    }

    return false;
}

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    if (record->event.pressed) {
        uprintf("Key pressed: %u (%s)\n", keycode, get_keycode_string_hlc(keycode));
    }
    return true;
}
