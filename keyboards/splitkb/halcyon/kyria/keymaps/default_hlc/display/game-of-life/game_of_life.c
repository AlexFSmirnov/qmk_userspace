#include <stdlib.h>
#include "game_of_life.h"
#include "display/utils.h"

#define DISPLAY_WIDTH 135
#define DISPLAY_HEIGHT 240

#define GOL_SQUARE_SIZE 3
#define KEYBOARD_PRESS_SQUARE_SIZE 7
#define KEYBOARD_PRESS_SQUARE_BUFFER 1

#define HEATMAP_SQUARE_SIZE 3
#define HEATMAP_MAX_HUE_CHANGE 60

// Calculate grid dimensions based on display size and square size
#define GRID_WIDTH (DISPLAY_WIDTH / GOL_SQUARE_SIZE)   // 27
#define GRID_HEIGHT (DISPLAY_HEIGHT / GOL_SQUARE_SIZE) // 48

// Row offsets for different layers (similar to draw_keymap.c)
static const int main_row_offsets[] = {3, 2, 0, 2, 4, 4};  // For rows 0-2 (6 columns each)
static const int thumb_row4_offsets[] = {3, 2};            // For row 3 (2 columns)
static const int thumb_row5_offsets[] = {3, 2, 1, 0, 0}; // For row 4 (5 columns)

// Static 2D boolean array to store the Game of Life grid
static bool gol_grid[GRID_HEIGHT][GRID_WIDTH];
static bool next_grid[GRID_HEIGHT][GRID_WIDTH];
static bool key_press_grid[GRID_HEIGHT][GRID_WIDTH];
static bool gol_initialized = false;

static bool has_pending_key_press = false;

// Function to initialize the grid with random values
static void initialize_gol_grid(void) {
    for (int y = 0; y < GRID_HEIGHT; y++) {
        for (int x = 0; x < GRID_WIDTH; x++) {
            // Generate random boolean value (0 or 1)
            gol_grid[y][x] = (rand() % 2) == 1;
        }
    }
    gol_initialized = true;
}

// Function to reset the grid back to random values
void reset_game_of_life_grid(void) {
    gol_initialized = false;
}

void register_game_of_life_key_press(int row, int col) {
    int display_row;
    int display_col;
    bool is_right_side;

    get_display_key_position(row, col, &display_row, &display_col, &is_right_side);

    int grid_y = display_row * (KEYBOARD_PRESS_SQUARE_SIZE);
    int grid_x = display_col * (KEYBOARD_PRESS_SQUARE_SIZE);

    // Apply row offsets based on the display layer (display_row)
    // Mirror the column index for left side (similar to draw_keymap.c)
    int row_offset = 0;
    int actual_col = is_right_side ? display_col : (6 - 1 - display_col);
    if (display_row >= 0 && display_row <= 2) {
        // Main keyboard layers (rows 0-2) with 6 columns each
        if (actual_col >= 0 && actual_col < 6) {
            row_offset = main_row_offsets[actual_col];
        }
    } else if (display_row == 3) {
        // Thumb layer row 3 with 2 columns
        if (actual_col >= 0 && actual_col < 2) {
            row_offset = thumb_row4_offsets[actual_col];
        }
    } else if (display_row == 4) {
        // Thumb layer row 4 with 5 columns
        if (actual_col >= 0 && actual_col < 5) {
            row_offset = thumb_row5_offsets[actual_col];
        }
    }

    grid_y += row_offset;

    if (is_right_side) {
        grid_y += GRID_HEIGHT / 2;
    }

    for (int i = -KEYBOARD_PRESS_SQUARE_BUFFER; i < KEYBOARD_PRESS_SQUARE_SIZE + KEYBOARD_PRESS_SQUARE_BUFFER; i++) {
        for (int j = -KEYBOARD_PRESS_SQUARE_BUFFER; j < KEYBOARD_PRESS_SQUARE_SIZE + KEYBOARD_PRESS_SQUARE_BUFFER; j++) {
            int gy = grid_y + i;
            int gx = grid_x + j;
            if (gy >= 0 && gy < GRID_HEIGHT && gx >= 0 && gx < GRID_WIDTH) {
                key_press_grid[gy][gx] = (rand() % 3) > 0;
            }
        }
    }

    has_pending_key_press = true;
}

void register_game_of_life_pixel(int x, int y) {
    int display_row = y / GOL_SQUARE_SIZE;
    int display_col = x / GOL_SQUARE_SIZE;

    key_press_grid[display_row][display_col] = true;
    has_pending_key_press = true;
}

void register_game_of_life_scroll(int shift) {
    // Initialize grid if not already done
    if (!gol_initialized) {
        initialize_gol_grid();
    }

    // Handle zero shift or no-op cases
    if (shift == 0) {
        return;
    }

    // Normalize shift to be within grid bounds
    shift = shift % GRID_HEIGHT;
    if (shift < 0) {
        shift += GRID_HEIGHT;
    }

    // Create a temporary grid to store the shifted result
    static bool temp_grid[GRID_HEIGHT][GRID_WIDTH];

    // Copy the current grid with the shift applied
    for (int y = 0; y < GRID_HEIGHT; y++) {
        for (int x = 0; x < GRID_WIDTH; x++) {
            // Calculate the source row with wrapping
            int source_y = (y + shift) % GRID_HEIGHT;
            temp_grid[y][x] = gol_grid[source_y][x];
        }
    }

    // Copy the result back to the main grid
    memcpy(gol_grid, temp_grid, sizeof(gol_grid));
}

void process_game_of_life_key_press(void) {
    if (!has_pending_key_press) {
        return;
    }

    for (int y = 0; y < GRID_HEIGHT; y++) {
        for (int x = 0; x < GRID_WIDTH; x++) {
            if (key_press_grid[y][x]) {
                gol_grid[y][x] = true;
                key_press_grid[y][x] = false;
            }
        }
    }

    has_pending_key_press = false;
}

void game_of_life_step(void) {
    // Initialize grid if not already done
    if (!gol_initialized) {
        initialize_gol_grid();
    }

    memcpy(next_grid, gol_grid, sizeof(gol_grid));

    for (int y = 0; y < GRID_HEIGHT; y++) {
        for (int x = 0; x < GRID_WIDTH; x++) {
            uint8_t alive_neighbors = 0;
            for (int i = -1; i <= 1; i++) {
                for (int j = -1; j <= 1; j++) {
                    if (i == 0 && j == 0) {
                        continue;
                    }

                    int ny = (y + i + GRID_HEIGHT) % GRID_HEIGHT;
                    int nx = (x + j + GRID_WIDTH) % GRID_WIDTH;

                    if (gol_grid[ny][nx]) {
                        alive_neighbors++;
                    }
                }
            }

            if (gol_grid[y][x]) {
                if (alive_neighbors < 2 || alive_neighbors > 3) {
                    next_grid[y][x] = false;
                }
            } else {
                if (alive_neighbors == 3) {
                    next_grid[y][x] = true;
                }
            }
        }
    }

    memcpy(gol_grid, next_grid, sizeof(gol_grid));

    process_game_of_life_key_press();
}

void draw_game_of_life_grid(painter_device_t surface) {
    qp_rect(surface, 0, 0, DISPLAY_WIDTH, DISPLAY_HEIGHT, 0, 0, 0, true);

    hsv_t hsv = rgb_matrix_config.hsv;

    for (int y = 0; y < GRID_HEIGHT; y++) {
        for (int x = 0; x < GRID_WIDTH; x++) {
            if (gol_grid[y][x]) {
                qp_rect(
                    surface,
                    x * GOL_SQUARE_SIZE,
                    y * GOL_SQUARE_SIZE,
                    x * GOL_SQUARE_SIZE + GOL_SQUARE_SIZE - 1,
                    y * GOL_SQUARE_SIZE + GOL_SQUARE_SIZE - 1,
                    hsv.h, hsv.s, 255,
                    1
                );
            }
        }
    }
}

void draw_game_of_life_heatmap(painter_device_t surface) {
    // Clear the display first
    qp_rect(surface, 0, 0, DISPLAY_WIDTH, DISPLAY_HEIGHT, 0, 0, 0, true);

    for (int y = 0; y < GRID_HEIGHT; y++) {
        for (int x = 0; x < GRID_WIDTH; x++) {
            // Count alive neighbors including the cell itself
            uint8_t alive_count = 0;

            for (int i = -HEATMAP_SQUARE_SIZE; i <= HEATMAP_SQUARE_SIZE; i++) {
                for (int j = -HEATMAP_SQUARE_SIZE; j <= HEATMAP_SQUARE_SIZE; j++) {
                    int ny = (y + i + GRID_HEIGHT) % GRID_HEIGHT;
                    int nx = (x + j + GRID_WIDTH) % GRID_WIDTH;

                    if (gol_grid[ny][nx]) {
                        alive_count++;
                    }
                }
            }

            // Only draw if there are alive cells in the neighborhood
            if (alive_count > 0 && gol_grid[y][x]) {
                // Map alive_count (0-9) to hue (0-240 degrees)
                // 0 = red (0°), 9 = blue (240°)
                uint8_t hue = (alive_count * 240) / (HEATMAP_SQUARE_SIZE * HEATMAP_SQUARE_SIZE * 2 + 1);

                // Use full saturation and moderate brightness for visibility
                uint8_t saturation = 255;
                uint8_t value = 200;

                qp_rect(
                    surface,
                    x * GOL_SQUARE_SIZE,
                    y * GOL_SQUARE_SIZE,
                    x * GOL_SQUARE_SIZE + GOL_SQUARE_SIZE - 1,
                    y * GOL_SQUARE_SIZE + GOL_SQUARE_SIZE - 1,
                    hue, saturation, value,
                    true
                );
            }
        }
    }
}

void draw_game_of_life_hue_heatmap(painter_device_t surface) {
    // Clear the display first
    qp_rect(surface, 0, 0, DISPLAY_WIDTH, DISPLAY_HEIGHT, 0, 0, 0, true);

    // Get current RGB matrix HSV values
    hsv_t base_hsv = rgb_matrix_config.hsv;

    for (int y = 0; y < GRID_HEIGHT; y++) {
        for (int x = 0; x < GRID_WIDTH; x++) {
            // Only process alive cells
            if (!gol_grid[y][x]) {
                continue;
            }

            // Count alive neighbors in the defined neighborhood
            uint8_t alive_count = 0;

            for (int i = -HEATMAP_SQUARE_SIZE; i <= HEATMAP_SQUARE_SIZE; i++) {
                for (int j = -HEATMAP_SQUARE_SIZE; j <= HEATMAP_SQUARE_SIZE; j++) {
                    int ny = (y + i + GRID_HEIGHT) % GRID_HEIGHT;
                    int nx = (x + j + GRID_WIDTH) % GRID_WIDTH;

                    if (gol_grid[ny][nx]) {
                        alive_count++;
                    }
                }
            }

            // Calculate hue shift based on neighbor density
            // Map alive_count to a hue increase from 0 to HEATMAP_MAX_HUE_CHANGE
            uint8_t max_possible_neighbors = (HEATMAP_SQUARE_SIZE * 2 + 1) * (HEATMAP_SQUARE_SIZE * 2 + 1);
            uint8_t hue_increase = (alive_count * HEATMAP_MAX_HUE_CHANGE) / max_possible_neighbors;

            // Apply hue shift to base hue (with wraparound)
            uint8_t final_hue = (base_hsv.h + hue_increase) % 256;

            qp_rect(
                surface,
                x * GOL_SQUARE_SIZE,
                y * GOL_SQUARE_SIZE,
                x * GOL_SQUARE_SIZE + GOL_SQUARE_SIZE - 1,
                y * GOL_SQUARE_SIZE + GOL_SQUARE_SIZE - 1,
                final_hue, base_hsv.s, 255,
                true
            );
        }
    }
}

bool process_game_of_life_display(painter_device_t surface) {
    static uint32_t last_update = 0;

    // Check if 100ms have elapsed since the last update
    if (timer_elapsed32(last_update) >= 50) {
        game_of_life_step();
        // draw_game_of_life_grid(surface);
        // draw_game_of_life_heatmap(surface);
        draw_game_of_life_hue_heatmap(surface);

        // Update the timestamp
        last_update = timer_read32();

        return true; // Indicate that the display was updated
    }

    return false; // No update occurred
}
