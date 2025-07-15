#include <stdlib.h>
#include "game_of_life.h"
#include "display/utils.h"

#define DISPLAY_WIDTH 135
#define DISPLAY_HEIGHT 240

#define GOL_SQUARE_SIZE 3
#define KEYBOARD_PRESS_SQUARE_SIZE 7
#define KEYBOARD_PRESS_SQUARE_BUFFER 3

// Calculate grid dimensions based on display size and square size
#define GRID_WIDTH (DISPLAY_WIDTH / GOL_SQUARE_SIZE)   // 27
#define GRID_HEIGHT (DISPLAY_HEIGHT / GOL_SQUARE_SIZE) // 48

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
    if (is_right_side) {
        grid_y += GRID_HEIGHT / 2;
    }

    for (int i = -KEYBOARD_PRESS_SQUARE_BUFFER; i < KEYBOARD_PRESS_SQUARE_SIZE + KEYBOARD_PRESS_SQUARE_BUFFER; i++) {
        for (int j = -KEYBOARD_PRESS_SQUARE_BUFFER; j < KEYBOARD_PRESS_SQUARE_SIZE + KEYBOARD_PRESS_SQUARE_BUFFER; j++) {
            int gy = grid_y + i;
            int gx = grid_x + j;
            if (gy >= 0 && gy < GRID_HEIGHT && gx >= 0 && gx < GRID_WIDTH) {
                key_press_grid[gy][gx] = (rand() % 2) == 1;
            }
        }
    }

    has_pending_key_press = true;
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

    process_game_of_life_key_press();

    memcpy(next_grid, gol_grid, sizeof(gol_grid));

    for (int y = 0; y < GRID_HEIGHT; y++) {
        for (int x = 0; x < GRID_WIDTH; x++) {
            uint8_t alive_neighbors = 0;
            for (int i = -1; i <= 1; i++) {
                for (int j = -1; j <= 1; j++) {
                    if (i == 0 && j == 0) {
                        continue;
                    }

                    int ny = y + i;
                    int nx = x + j;

                    if (ny < 0 || ny >= GRID_HEIGHT || nx < 0 || nx >= GRID_WIDTH) {
                        continue;
                    }

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

bool process_game_of_life_display(painter_device_t surface) {
    static uint32_t last_update = 0;

    // Check if 100ms have elapsed since the last update
    if (timer_elapsed32(last_update) >= 50) {
        game_of_life_step();
        draw_game_of_life_grid(surface);

        // Update the timestamp
        last_update = timer_read32();

        return true; // Indicate that the display was updated
    }

    return false; // No update occurred
}
