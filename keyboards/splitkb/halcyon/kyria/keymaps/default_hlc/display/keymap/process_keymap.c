#include "process_keymap.h"
#include "keycode_strings.h"
#include "draw_keymap.h"

// Include the keymap data
extern const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS];

// Function to get the active keymap as strings
void get_active_keymap_strings(const char* keymap_strings[5][6], bool is_right_side) {
    // Get the current active layer
    uint8_t active_layer = get_highest_layer(layer_state | default_layer_state);

    // Read keymap data from PROGMEM and convert to strings
    // For left side: read rows 0-4, for right side: read rows 5-9
    int start_row = is_right_side ? 5 : 0;

    for (int row = 0; row < 5; row++) {
        for (int col = 0; col < 6; col++) {
            int keymap_col = col;
            int keymap_row = row;

            // Thumb keys are in a weird order
            if (is_right_side) {
                if (row == 3) {
                    if (col == 0) {
                        keymap_row = 2;
                        keymap_col = -1;
                    }
                    if (col == 1) {
                        keymap_row = 3;
                        keymap_col = 2;
                    }
                }

                if (row == 4) {
                    keymap_row = 3;
                    switch (col) {
                        case 0: keymap_col = -1; break;
                        case 1: keymap_col = 4; break;
                        case 2: keymap_col = 0; break;
                        case 3: keymap_col = 1; break;
                        case 4: keymap_col = 3; break;
                    }
                }
            } else {
                if (row == 3) {
                    if (col == 5) {
                        keymap_row = 2;
                        keymap_col = 6;
                    }
                    if (col == 4) {
                        keymap_row = 3;
                        keymap_col = 3;
                    }
                }

                if (row == 4) {
                    keymap_row = 3;
                    // keymap_col = col;
                    switch (col) {
                        case 1: keymap_col = 2; break;
                        case 2: keymap_col = 4; break;
                        case 3: keymap_col = 5; break;
                        case 4: keymap_col = 1; break;
                        case 5: keymap_col = 6; break;
                    }
                }
            }

            // For the left side, reverse the column order
            keymap_col = is_right_side ? (keymap_col + 1) : (6 - keymap_col);

            // Read the keycode from the keymap array
            uint16_t keycode = pgm_read_word(&keymaps[active_layer][start_row + keymap_row][keymap_col]);

            // Convert keycode to string using the existing function
            const char* key_string = get_keycode_string_hlc(keycode);

            // Store the string in our array
            keymap_strings[row][col] = key_string;
        }
    }
}

void draw_keymap_layout(painter_device_t surface) {
    // Create arrays to hold the active keymap strings for left and right sides
    const char* left_keymap[5][6];
    const char* right_keymap[5][6];

    // Get the current active keymap as strings for both sides
    get_active_keymap_strings(left_keymap, false);  // Left side
    get_active_keymap_strings(right_keymap, true);  // Right side

    // Draw the layout grid with the actual keymap
    draw_layout_grid(surface, 0, 0, true, left_keymap);
    draw_layout_grid(surface, 0, 125, false, right_keymap);
}

bool should_redraw_keymap(layer_state_t last_layer_state, uint8_t last_default_layer) {
    // Check if layer state has changed
    layer_state_t current_layer_state = layer_state | default_layer_state;
    uint8_t current_default_layer = get_highest_layer(default_layer_state);

    return (current_layer_state != last_layer_state || current_default_layer != last_default_layer);
}

void update_layer_states(layer_state_t* last_layer_state, uint8_t* last_default_layer) {
    *last_layer_state = layer_state | default_layer_state;
    *last_default_layer = get_highest_layer(default_layer_state);
}
