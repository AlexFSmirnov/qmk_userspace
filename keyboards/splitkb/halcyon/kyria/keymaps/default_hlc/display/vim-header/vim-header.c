#include "vim-header.h"
#include "../../qmk-vim/modes.h"
#include "../../transactions/vim_mode_sync.h"
#include "../fonts/12x20.qff.h"
#include "rgb_matrix.h"

#define BANNER_HEIGHT 30

// Font handle for the header text
static painter_font_handle_t font_12x20_handle = NULL;

static bool was_reset = true;
static vim_mode_t previous_mode = NORMAL_MODE;
static uint8_t previous_layer = 0;

bool process_vim_header_display(painter_device_t surface) {
    // Get current mode
    vim_mode_t current_mode = get_synced_vim_mode();
    uint8_t active_layer = get_highest_layer(layer_state | default_layer_state);

    // Only proceed if mode changed or was reset
    if (!was_reset && current_mode == previous_mode && active_layer == previous_layer) {
        return false;
    }

    // Load font if not already loaded
    if (font_12x20_handle == NULL) {
        font_12x20_handle = qp_load_font_mem(font_12x20);
        if (font_12x20_handle == NULL) {
            uprintf("font_12x20_handle is NULL\n");
            return false;
        }
    }

    // Get the display dimensions
    uint16_t panel_width = qp_get_width(surface);

    // Set up banner color based on vim mode
    HSV hsv = rgb_matrix_config.hsv;
    hsv.v = 255;
    switch (current_mode) {
        case NORMAL_MODE:
            hsv.h = 140;
            break;
        case INSERT_MODE:
            hsv.h = 100;
            break;
        case VISUAL_MODE:
        case VISUAL_LINE_MODE:
            hsv.h = 35;
            break;
        default:
            break;
    }

    // Draw the banner rectangle at the top of the screen
    qp_rect(surface, 0, 0, panel_width - 1, BANNER_HEIGHT - 1, hsv.h, hsv.s, hsv.v, 1);

    // Get the mode text
    const char* mode_text;
    switch (current_mode) {
        case NORMAL_MODE:
            mode_text = "NORMAL";
            break;
        case INSERT_MODE:
            mode_text = "INSERT";
            break;
        case VISUAL_MODE:
            mode_text = "VISUAL";
            break;
        case VISUAL_LINE_MODE:
            mode_text = "V-LINE";
            break;
        default:
            mode_text = "UNKNOWN";
            break;
    }

    // Calculate text position (centered horizontally and vertically)
    int16_t text_width = qp_textwidth(font_12x20_handle, mode_text);
    int16_t text_height = font_12x20_handle->line_height;

    int text_x = (panel_width - text_width) / 2;
    int text_y = (BANNER_HEIGHT - text_height) / 2;

    // Draw the text in black
    qp_drawtext_recolor(surface, text_x, text_y, font_12x20_handle, mode_text, HSV_BLACK, hsv.h, hsv.s, hsv.v);

    // Update state for next call
    previous_mode = current_mode;
    was_reset = false;
    previous_layer = active_layer;

    return true;
}

void clear_vim_header(void) {
    was_reset = true;
}
