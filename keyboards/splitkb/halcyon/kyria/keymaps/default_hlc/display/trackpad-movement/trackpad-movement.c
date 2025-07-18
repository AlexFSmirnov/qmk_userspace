#include "trackpad-movement.h"
#include "quantum.h"

// Display dimensions from controller.c
#define LCD_WIDTH 135
#define LCD_HEIGHT 240

// Render cooldown period in milliseconds
#define RENDER_COOLDOWN_MS 200

// Static variables to track state
static bool pixel_buffer[LCD_HEIGHT][LCD_WIDTH];
static int last_x = LCD_WIDTH / 2;   // Default to center (67)
static int last_y = LCD_HEIGHT / 2;  // Default to center (120)
static bool buffer_initialized = false;
static bool buffer_dirty = false;    // Track if buffer has changes since last render
static uint32_t last_render_time = 0; // Track when we last rendered

// Initialize pixel buffer to all false
static void init_pixel_buffer(void) {
    if (buffer_initialized) return;

    for (int y = 0; y < LCD_HEIGHT; y++) {
        for (int x = 0; x < LCD_WIDTH; x++) {
            pixel_buffer[y][x] = false;
        }
    }
    buffer_initialized = true;
}

// Bresenham's line algorithm to draw line from (x0,y0) to (x1,y1)
static void draw_line_to_buffer(int x0, int y0, int x1, int y1) {
    int dx = abs(x1 - x0);
    int dy = abs(y1 - y0);
    int sx = x0 < x1 ? 1 : -1;
    int sy = y0 < y1 ? 1 : -1;
    int err = dx - dy;

    int x = x0;
    int y = y0;

    while (true) {
        // Set pixel if within bounds
        if (x >= 0 && x < LCD_WIDTH && y >= 0 && y < LCD_HEIGHT) {
            pixel_buffer[y][x] = true;
        }

        // Check if we've reached the end point
        if (x == x1 && y == y1) break;

        int e2 = 2 * err;
        if (e2 > -dy) {
            err -= dy;
            x += sx;
        }
        if (e2 < dx) {
            err += dx;
            y += sy;
        }
    }
}

void clear_trackpad_movement(void) {
    init_pixel_buffer();

    // Clear all pixels in buffer
    for (int y = 0; y < LCD_HEIGHT; y++) {
        for (int x = 0; x < LCD_WIDTH; x++) {
            pixel_buffer[y][x] = false;
        }
    }

    // Reset position to center
    last_x = LCD_WIDTH / 2;
    last_y = LCD_HEIGHT / 2;

    // Mark buffer as dirty since we cleared it
    buffer_dirty = true;
}

void register_trackpad_movement(int x, int y) {
    init_pixel_buffer();

    // Calculate new position from relative movement
    int new_x = last_x + x;
    int new_y = last_y + y;

    // Check if wrapping occurs and handle line drawing accordingly
    bool wrapped_x = false;
    bool wrapped_y = false;

    int final_x = new_x;
    int final_y = new_y;

    // Handle X wrapping
    if (new_x < 0) {
        wrapped_x = true;
        while (final_x < 0) final_x += LCD_WIDTH;
    } else if (new_x >= LCD_WIDTH) {
        wrapped_x = true;
        while (final_x >= LCD_WIDTH) final_x -= LCD_WIDTH;
    }

    // Handle Y wrapping
    if (new_y < 0) {
        wrapped_y = true;
        while (final_y < 0) final_y += LCD_HEIGHT;
    } else if (new_y >= LCD_HEIGHT) {
        wrapped_y = true;
        while (final_y >= LCD_HEIGHT) final_y -= LCD_HEIGHT;
    }

    if (wrapped_x || wrapped_y) {
        // Wrapping occurred, need to split the line

        if (wrapped_x && !wrapped_y) {
            // Only X wrapped
            if (x != 0) {  // Avoid division by zero
                if (new_x < 0) {
                    // Wrapped to left, draw to left edge then from right edge
                    int edge_y = last_y + (0 - last_x) * y / x;
                    // Clamp edge_y to valid bounds
                    if (edge_y < 0) edge_y = 0;
                    if (edge_y >= LCD_HEIGHT) edge_y = LCD_HEIGHT - 1;
                    draw_line_to_buffer(last_x, last_y, 0, edge_y);
                    draw_line_to_buffer(LCD_WIDTH - 1, edge_y, final_x, final_y);
                } else {
                    // Wrapped to right, draw to right edge then from left edge
                    int edge_y = last_y + (LCD_WIDTH - 1 - last_x) * y / x;
                    // Clamp edge_y to valid bounds
                    if (edge_y < 0) edge_y = 0;
                    if (edge_y >= LCD_HEIGHT) edge_y = LCD_HEIGHT - 1;
                    draw_line_to_buffer(last_x, last_y, LCD_WIDTH - 1, edge_y);
                    draw_line_to_buffer(0, edge_y, final_x, final_y);
                }
            } else {
                // Vertical movement only
                draw_line_to_buffer(last_x, last_y, final_x, final_y);
            }
        } else if (wrapped_y && !wrapped_x) {
            // Only Y wrapped
            if (y != 0) {  // Avoid division by zero
                if (new_y < 0) {
                    // Wrapped to top, draw to top edge then from bottom edge
                    int edge_x = last_x + (0 - last_y) * x / y;
                    // Clamp edge_x to valid bounds
                    if (edge_x < 0) edge_x = 0;
                    if (edge_x >= LCD_WIDTH) edge_x = LCD_WIDTH - 1;
                    draw_line_to_buffer(last_x, last_y, edge_x, 0);
                    draw_line_to_buffer(edge_x, LCD_HEIGHT - 1, final_x, final_y);
                } else {
                    // Wrapped to bottom, draw to bottom edge then from top edge
                    int edge_x = last_x + (LCD_HEIGHT - 1 - last_y) * x / y;
                    // Clamp edge_x to valid bounds
                    if (edge_x < 0) edge_x = 0;
                    if (edge_x >= LCD_WIDTH) edge_x = LCD_WIDTH - 1;
                    draw_line_to_buffer(last_x, last_y, edge_x, LCD_HEIGHT - 1);
                    draw_line_to_buffer(edge_x, 0, final_x, final_y);
                }
            } else {
                // Horizontal movement only
                draw_line_to_buffer(last_x, last_y, final_x, final_y);
            }
        } else {
            // Both X and Y wrapped - this is complex, for now just draw to final position
            // TODO: Handle diagonal wrapping properly
            draw_line_to_buffer(last_x, last_y, final_x, final_y);
        }
    } else {
        // No wrapping, draw normal line
        draw_line_to_buffer(last_x, last_y, final_x, final_y);
    }

    // Update last known position
    last_x = final_x;
    last_y = final_y;

    // Mark buffer as dirty since we added movement
    buffer_dirty = true;
}

bool process_trackpad_movement_display(painter_device_t surface) {
    init_pixel_buffer();

    // If no changes since last render, return false
    if (!buffer_dirty) {
        return false;
    }

    // Check if enough time has passed since last render
    uint32_t current_time = timer_read32();
    if ((current_time - last_render_time) < RENDER_COOLDOWN_MS) {
        // Still in cooldown period, don't render yet but keep buffer dirty
        // return false;
    }

    // Clear the surface first
    // qp_rect(surface, 0, 0, LCD_WIDTH, LCD_HEIGHT, 0, 0, 0, true);

    // Get current RGB matrix HSV for consistent coloring
    hsv_t hsv = rgb_matrix_config.hsv;

    // Draw all pixels that are marked as "on" in our buffer
    for (int y = 0; y < LCD_HEIGHT; y++) {
        for (int x = 0; x < LCD_WIDTH; x++) {
            if (pixel_buffer[y][x]) {
                // Draw a single pixel using a 1x1 rectangle
                qp_rect(surface, x, y, x, y, hsv.h, hsv.s, 255, true);
                // Clear the pixel from the buffer since we've drawn it
                pixel_buffer[y][x] = false;
            }
        }
    }

    // Mark buffer as clean since we just rendered it
    buffer_dirty = false;

    // Update last render time
    last_render_time = current_time;

    return true;
}
