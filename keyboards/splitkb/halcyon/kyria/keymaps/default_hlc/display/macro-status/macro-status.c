#include "macro-status.h"
#include "../../macros/macro_recorder.h"
#include "../fonts/9x12.qff.h"
#include <stdio.h>

#define LCD_WIDTH 135
#define LCD_HEIGHT 240
#define MACRO_STATUS_Y 20

static painter_font_handle_t font_9x12_handle = NULL;
static bool last_recording_state = false;
static uint16_t last_action_count = 0;

void init_macro_status_display(void) {
    last_recording_state = false;
    last_action_count = 0;
}

void clear_macro_status_display(void) {
    last_recording_state = false;
    last_action_count = 0;
}

bool process_macro_status_display(painter_device_t surface) {
    bool is_recording = macro_recorder_is_recording();
    uint16_t action_count = macro_recorder_get_recorded_count();

    // Check if state changed
    bool state_changed = (is_recording != last_recording_state) || (action_count != last_action_count);

    if (!is_recording) {
        // If we just stopped recording, trigger one more redraw to clear
        if (last_recording_state) {
            last_recording_state = false;
            last_action_count = 0;
            return true;
        }
        return false;
    }

    if (!state_changed) {
        return false;
    }

    // Load font if not already loaded
    if (font_9x12_handle == NULL) {
        font_9x12_handle = qp_load_font_mem(font_9x12);
        if (font_9x12_handle == NULL) {
            return false;
        }
    }

    // Prepare the recording status text
    char status_text[64];
    uint8_t slot = macro_recorder_get_current_slot();
    snprintf(status_text, sizeof(status_text), "REC M%d: %d", slot + 1, action_count);

    // Calculate text dimensions
    int16_t text_width = qp_textwidth(font_9x12_handle, status_text);
    int16_t text_height = font_9x12_handle->line_height;

    // Center horizontally, near top
    int text_x = (LCD_WIDTH - text_width) / 2;
    int text_y = MACRO_STATUS_Y;

    // Define background rectangle with padding
    int padding = 3;
    int bg_x = text_x - padding;
    int bg_y = text_y - padding;
    int bg_width = text_width + (padding * 2);
    int bg_height = text_height + (padding * 2);

    // Ensure background stays within screen bounds
    if (bg_x < 0) bg_x = 0;
    if (bg_y < 0) bg_y = 0;
    if (bg_x + bg_width > LCD_WIDTH) bg_width = LCD_WIDTH - bg_x;
    if (bg_y + bg_height > LCD_HEIGHT) bg_height = LCD_HEIGHT - bg_y;

    // Draw background rectangle (red to indicate recording)
    qp_rect(surface, bg_x, bg_y, bg_x + bg_width - 1, bg_y + bg_height - 1, HSV_RED, true);

    // Draw border
    qp_rect(surface, bg_x, bg_y, bg_x + bg_width - 1, bg_y + bg_height - 1, HSV_WHITE, false);

    // Draw the status text in white
    qp_drawtext_recolor(surface, text_x, text_y, font_9x12_handle, status_text, 0, 0, 255, HSV_RED);

    // Update last state
    last_recording_state = is_recording;
    last_action_count = action_count;

    return true;
}

