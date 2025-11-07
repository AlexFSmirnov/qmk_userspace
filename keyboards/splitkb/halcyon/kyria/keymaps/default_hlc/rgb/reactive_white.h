#ifdef RGB_MATRIX_KEYREACTIVE_ENABLED
RGB_MATRIX_EFFECT(REACTIVE_WHITE)
#        ifdef RGB_MATRIX_CUSTOM_EFFECT_IMPLS
#include "enums.h"
#include "../macros/macro_recorder.h"

extern const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS];

static void set_underglow_color(uint8_t h, uint8_t s, uint8_t v) {
    hsv_t hsv = {(h + 256 - 8) % 256, s, v};
    rgb_t rgb = rgb_matrix_hsv_to_rgb(hsv);
    for (uint8_t i = 0; i < 6; i++) {
        rgb_matrix_set_color(i, rgb.r, rgb.g, rgb.b);
    }
}

bool REACTIVE_WHITE_base(effect_params_t* params) {
    RGB_MATRIX_USE_LIMITS(led_min, led_max);

    uint16_t max_tick = 65535 / qadd8(rgb_matrix_config.speed, 1);
    for (uint8_t i = led_min; i < led_max; i++) {
        RGB_MATRIX_TEST_LED_FLAGS();
        uint16_t tick = max_tick;
        // Reverse search to find most recent key hit
        for (int8_t j = g_last_hit_tracker.count - 1; j >= 0; j--) {
            if (g_last_hit_tracker.index[j] == i && g_last_hit_tracker.tick[j] < tick) {
                tick = g_last_hit_tracker.tick[j];
                break;
            }
        }

        uint16_t offset = scale16by8(tick, qadd8(rgb_matrix_config.speed, 1));

        hsv_t hsv = rgb_matrix_config.hsv;
        if (offset != 257) {
            uint8_t min_s = scale8(hsv.s, 120);
            hsv.s = hsv.s - scale8(255 - offset, hsv.s - min_s);
        }
        rgb_t rgb = rgb_matrix_hsv_to_rgb(hsv);
        rgb_matrix_set_color(i, rgb.r, rgb.g, rgb.b);
    }

    set_underglow_color(rgb_matrix_config.hsv.h, rgb_matrix_config.hsv.s, 200);

    return rgb_matrix_check_finished_leds(led_max);
}

static bool LAYER_COLUMNS(effect_params_t* params) {
    RGB_MATRIX_USE_LIMITS(led_min, led_max);

    hsv_t on_hsv = rgb_matrix_config.hsv;
    hsv_t off_hsv = rgb_matrix_config.hsv;
    off_hsv.s = 0;

    rgb_t on_rgb = rgb_matrix_hsv_to_rgb(on_hsv);
    for (uint8_t i = led_min; i <= led_max; i++) {
        rgb_matrix_set_color(i, on_rgb.r, on_rgb.g, on_rgb.b);
    }

    for (uint8_t row = 0; row < 9; row++) {
        for (uint8_t col = 0; col < 7; col++) {
            uint8_t led = g_led_config.matrix_co[row][col];
            hsv_t hsv = ((col + 1) % 4 < 2) ? off_hsv : on_hsv;

            if (row == 8) {
                if (col == 0 || col == 3 || col == 5 || col == 4) {
                    hsv = off_hsv;
                } else {
                    hsv = on_hsv;
                }
            }

            if (row == 3 && col == 5) {
                hsv = off_hsv;
            }

            if (led >= led_min && led <= led_max) {
                rgb_t rgb = rgb_matrix_hsv_to_rgb(hsv);
                rgb_matrix_set_color(led, rgb.r, rgb.g, rgb.b);
            }
        }
    }

    set_underglow_color(on_hsv.h, on_hsv.s, 200);

    return rgb_matrix_check_finished_leds(led_max);
}

static bool MOUSE_KEYS(effect_params_t* params) {
    RGB_MATRIX_USE_LIMITS(led_min, led_max);

    hsv_t on_hsv = rgb_matrix_config.hsv;
    hsv_t off_hsv = rgb_matrix_config.hsv;
    off_hsv.s = 0;

    uint8_t active_layer = get_highest_layer(layer_state | default_layer_state);

    for (uint8_t row = 0; row < 9; row++) {
        for (uint8_t col = 0; col < 7; col++) {
            uint8_t led = g_led_config.matrix_co[row][col];

            uint16_t keycode = pgm_read_word(&keymaps[active_layer][row][col]);

            hsv_t hsv = (keycode == MS_BTN1 || keycode == MS_BTN2 || keycode == MS_BTN3) ? off_hsv : on_hsv;

            if (led >= led_min && led <= led_max) {
                rgb_t rgb = rgb_matrix_hsv_to_rgb(hsv);
                rgb_matrix_set_color(led, rgb.r, rgb.g, rgb.b);
            }
        }
    }

    set_underglow_color(on_hsv.h, on_hsv.s, 200);

    return rgb_matrix_check_finished_leds(led_max);
}

static bool GAMING_LAYER(effect_params_t* params) {
    RGB_MATRIX_USE_LIMITS(led_min, led_max);

    uint16_t max_tick = 65535 / qadd8(rgb_matrix_config.speed, 1);
    for (uint8_t i = led_min; i < led_max; i++) {
        RGB_MATRIX_TEST_LED_FLAGS();
        uint16_t tick = max_tick;
        // Reverse search to find most recent key hit
        for (int8_t j = g_last_hit_tracker.count - 1; j >= 0; j--) {
            if (g_last_hit_tracker.index[j] == i && g_last_hit_tracker.tick[j] < tick) {
                tick = g_last_hit_tracker.tick[j];
                break;
            }
        }

        uint16_t offset = scale16by8(tick, qadd8(rgb_matrix_config.speed, 1));

        hsv_t hsv = rgb_matrix_config.hsv;
        if (offset != 257) {
            uint8_t min_s = scale8(hsv.s, 120);
            hsv.s = hsv.s - scale8(255 - offset, hsv.s - min_s);
        }

        // Check if this LED corresponds to WASD keys in gaming layer
        // W is at row 0, col 3 (4th position in gaming layer)
        // A is at row 1, col 2 (3rd position in gaming layer)
        // S is at row 1, col 3 (4th position in gaming layer)
        // D is at row 1, col 4 (5th position in gaming layer)
        uint8_t led_row = 255, led_col = 255;
        for (uint8_t row = 0; row < MATRIX_ROWS; row++) {
            for (uint8_t col = 0; col < MATRIX_COLS; col++) {
                if (g_led_config.matrix_co[row][col] == i) {
                    led_row = row;
                    led_col = col;
                    break;
                }
            }
            if (led_row != 255) break;
        }

                // Set WASD keys to white in gaming layer
        if ((led_row == 0 && led_col == 3) ||  // W
            (led_row == 1 && led_col == 2) ||  // A
            (led_row == 1 && led_col == 3) ||  // S
            (led_row == 1 && led_col == 4)) {  // D
            // Create white color at the same brightness as other keys
            hsv_t white_hsv = {0, 0, hsv.v}; // Hue=0, Saturation=0 (white), same Value/brightness
            rgb_t white_rgb = rgb_matrix_hsv_to_rgb(white_hsv);
            rgb_matrix_set_color(i, white_rgb.r, white_rgb.g, white_rgb.b);
        } else {
            rgb_t rgb = rgb_matrix_hsv_to_rgb(hsv);
            rgb_matrix_set_color(i, rgb.r, rgb.g, rgb.b);
        }
    }

    set_underglow_color(rgb_matrix_config.hsv.h, rgb_matrix_config.hsv.s, 200);

    return rgb_matrix_check_finished_leds(led_max);
}

static bool MACRO_LAYER(effect_params_t* params) {
    RGB_MATRIX_USE_LIMITS(led_min, led_max);

    hsv_t on_hsv = {0, 0, 255};  // White for PLAY keys with macros
    hsv_t off_hsv = rgb_matrix_config.hsv;  // Normal color for keys without macros
    hsv_t recording_hsv = {0, 255, 255};  // Red for currently recording
    hsv_t empty_hsv = {85, 255, 255};  // Green for empty REC slots
    hsv_t has_content_hsv = {43, 255, 255};  // Yellow for REC slots with content (warning)

    uint8_t active_layer = get_highest_layer(layer_state | default_layer_state);
    bool is_recording = macro_recorder_is_recording();
    uint8_t recording_slot = is_recording ? macro_recorder_get_current_slot() : 0;

    for (uint8_t row = 0; row < 9; row++) {
        for (uint8_t col = 0; col < 7; col++) {
            uint8_t led = g_led_config.matrix_co[row][col];

            uint16_t keycode = pgm_read_word(&keymaps[active_layer][row][col]);

            hsv_t hsv = off_hsv;

            // Check if this is a macro key
            if (keycode >= MACRO_REC_1 && keycode <= MACRO_REC_10) {
                uint8_t slot = keycode - MACRO_REC_1;
                if (is_recording && slot == recording_slot) {
                    // Red: Currently recording this slot
                    hsv = recording_hsv;
                } else if (macro_recorder_slot_has_content(slot)) {
                    // Yellow: Has content (will overwrite)
                    hsv = has_content_hsv;
                } else {
                    // Green: Empty, ready to record
                    hsv = empty_hsv;
                }
            } else if (keycode >= MACRO_PLAY_1 && keycode <= MACRO_PLAY_10) {
                uint8_t slot = keycode - MACRO_PLAY_1;
                // PLAY keys: White if macro exists
                if (macro_recorder_slot_has_content(slot)) {
                    hsv = on_hsv;
                }
            } else if (keycode >= MACRO_CLEAR_1 && keycode <= MACRO_CLEAR_10) {
                uint8_t slot = keycode - MACRO_CLEAR_1;
                // CLEAR keys: Red when there's content to clear
                if (macro_recorder_slot_has_content(slot)) {
                    hsv = recording_hsv;  // Red to indicate destructive action
                }
            }

            if (led >= led_min && led <= led_max) {
                rgb_t rgb = rgb_matrix_hsv_to_rgb(hsv);
                rgb_matrix_set_color(led, rgb.r, rgb.g, rgb.b);
            }
        }
    }

    set_underglow_color(off_hsv.h, off_hsv.s, 200);

    return rgb_matrix_check_finished_leds(led_max);
}

bool REACTIVE_WHITE(effect_params_t* params) {
    uint8_t active_layer = get_highest_layer(layer_state | default_layer_state);
    if (active_layer == 0) {
        return REACTIVE_WHITE_base(params);
    }

    if (active_layer == _GAMING) {
        return GAMING_LAYER(params);
    }

    if (active_layer == _MOUSE_KEYS) {
        return MOUSE_KEYS(params);
    }

    if (active_layer == _MACROS) {
        return MACRO_LAYER(params);
    }

    return LAYER_COLUMNS(params);
}

#        endif // RGB_MATRIX_CUSTOM_EFFECT_IMPLS
#endif         // RGB_MATRIX_KEYREACTIVE_ENABLED
