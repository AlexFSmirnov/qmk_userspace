#include "qp.h"
#include "quantum.h"
#include "keycode_strings.h"
#include "hlc_tft_display/hlc_tft_display.h"
#include "qp_surface.h"
#include "display/draw_layout.h"

#define LCD_WIDTH 135
#define LCD_HEIGHT 240

painter_device_t lcd;
painter_device_t lcd_surface;

// Sample keymap with numbers 1-6 for each row
static const char* sample_keymap[5][6] = {
    {"11", "22", "33", "44", "55", "66"},  // Row 0
    {"1", "2", "3", "4", "5", "6"},  // Row 1
    {"111", "222", "333", "444", "555", "666"},  // Row 2
    {"1111", "2222", "3333", "4444", "5555", "6666"},  // Row 3
    {"11111", "22222", "33333", "44444", "55555", "66666"}   // Row 4
};

bool module_post_init_user(void) {
    // Draw the layout grid
    draw_layout_grid(lcd_surface, 0, 0, true, sample_keymap);
    draw_layout_grid(lcd_surface, 0, 125, false, sample_keymap);

    qp_surface_draw(lcd_surface, lcd, 0, 0, 0);

    // int16_t width = qp_textwidth(my_font, text);
    // qp_drawtext_recolor(lcd_surface, 0, (LCD_HEIGHT - my_font->line_height), my_font, text, HSV_BLUE, HSV_BLACK);
    // qp_surface_draw(lcd_surface, lcd, 0, 0, 0);
    // qp_rect(lcd, 20, 10, 40, 30, HSV_RED, 1);

    return false;
}

bool display_module_housekeeping_task_user(bool second_display) {
    return false;
}

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    if (record->event.pressed) {
        uprintf("Key pressed: %u (%s)\n", keycode, get_keycode_string_hlc(keycode));

    }
    return true;
}
