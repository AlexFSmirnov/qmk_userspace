#include "qp.h"
#include "draw_layout.h"
#include "fonts/3x5.qff.h"
#include "fonts/4x7.qff.h"
#include "fonts/5x7.qff.h"
#include "fonts/9x12.qff.h"

#define BORDER_COLOR 0, 0, 100

// Offsets array for column positioning
static const int offsets[] = {10, 7, 0, 5, 20, 18};
static const int thumb_offsets[] = {10, 7, 4, 2, 2};
static const int square_width = 22;
static const int square_height = 20;

// Prepare fonts
static painter_font_handle_t font_9x12_handle = NULL;
static painter_font_handle_t font_5x7_handle = NULL;
static painter_font_handle_t font_4x7_handle = NULL;
static painter_font_handle_t font_3x5_handle = NULL;

// Function to draw a key label at the specified position
void draw_key_label(painter_device_t surface, int x, int y, const char* label) {
    // Load font if not already loaded
    if (font_9x12_handle == NULL) {
        font_9x12_handle = qp_load_font_mem(font_9x12);
    }
    if (font_5x7_handle == NULL) {
        font_5x7_handle = qp_load_font_mem(font_5x7);
    }
    if (font_4x7_handle == NULL) {
        font_4x7_handle = qp_load_font_mem(font_4x7);
    }
    if (font_3x5_handle == NULL) {
        font_3x5_handle = qp_load_font_mem(font_3x5);
    }

    const painter_font_handle_t font_handle =
        (label && label[0] && !label[1]) ? font_9x12_handle : // 1 char
        (label && label[0] && label[1] && !label[2]) ? font_9x12_handle : // 2 chars
        (label && label[0] && label[1] && label[2] && !label[3]) ? font_5x7_handle : // 3 chars
        (label && label[0] && label[1] && label[2] && label[3] && !label[4]) ? font_4x7_handle : // 4 chars
        font_3x5_handle; // 5 or more chars

    // Calculate center position for the text within the key square
    int16_t text_width = qp_textwidth(font_handle, label);
    int16_t text_height = font_handle->line_height;

    // Center the text horizontally and vertically within the key square
    int text_x = x + (square_width - text_width) / 2;
    int text_y = y + (square_height - text_height) / 2 + 1;

    // Draw the text
    qp_drawtext_recolor(surface, text_x, text_y, font_handle, label, HSV_WHITE, HSV_BLACK);
}

// Function to draw a complete key (clear, label, border)
void draw_key(painter_device_t surface, int x, int y, const char* label) {
    // Clear the rectangle
    qp_rect(surface, x, y, x + square_width, y + square_height, HSV_BLACK, 1);

    // Draw the label
    draw_key_label(surface, x, y, label);

    // Draw the border
    qp_rect(surface, x, y, x + square_width, y + square_height, BORDER_COLOR, 0);
}

// Function to draw a 3x5 grid layout
void draw_layout_grid(painter_device_t surface, int x, int y, bool mirrored, const char* keymap[5][6]) {
    const int columns = 6;
    const int rows = 3;

    // For right side (mirrored = false), adjust the x offset to position correctly
    int x_offset = x;
    if (!mirrored) {
        // Right side should be positioned to show correct physical layout
        // Based on the layout, right side keys start at x=8.5, so we need to adjust
        x_offset = x + 2; // Adjust this value to position correctly
    }

    for (int col = 0; col < columns; col++) {
        int actual_col = mirrored ? (columns - 1 - col) : col;
        int col_x = x_offset + (actual_col * square_width);
        int col_y = y + offsets[col];

        for (int row = 0; row < rows; row++) {
            int square_y = col_y + (row * square_height);
            draw_key(surface, col_x, square_y, keymap[row][actual_col]);
        }
    }

    // Thumb square buttons
    const int thumb_row_offset = 4;
    for (int col = 0; col < 2; col++) {
        int actual_col = mirrored ? (columns - 1 - col) : col; // Reverse order when mirrored
        int col_x = x_offset + (actual_col * square_width);
        for (int row = 0; row < 2; row++) {
            int square_y = y + rows * square_height + thumb_offsets[col] + (row * square_height) + thumb_row_offset;
            draw_key(surface, col_x, square_y, keymap[row + 3][actual_col]);
        }
    }

    // Rest of thumb buttons
    for (int col = 2; col < 5; col++) {
        int actual_col = mirrored ? (columns - 1 - col) : col; // Reverse order when mirrored
        int col_x = x_offset + (actual_col * square_width);
        int square_y = y + rows * square_height + thumb_offsets[col] + square_height + thumb_row_offset;
        draw_key(surface, col_x, square_y, keymap[4][actual_col]);
    }
}
