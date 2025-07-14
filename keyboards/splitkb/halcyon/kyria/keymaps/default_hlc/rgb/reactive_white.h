#ifdef RGB_MATRIX_KEYREACTIVE_ENABLED
RGB_MATRIX_EFFECT(REACTIVE_WHITE)
#        ifdef RGB_MATRIX_CUSTOM_EFFECT_IMPLS

static hsv_t REACTIVE_WHITE_math(hsv_t hsv, uint16_t offset) {
#            ifdef RGB_MATRIX_REACTIVE_WHITE_GRADIENT_MODE
    hsv.h = scale16by8(g_rgb_timer, qadd8(rgb_matrix_config.speed, 8) >> 4);
#            endif
    if (offset == 257) {
        return hsv;
    }

    uint8_t min_s = scale8(hsv.s, 120);
    hsv.s = hsv.s - scale8(255 - offset, hsv.s - min_s);
    // hsv.h += scale8(255 - offset, 64);
    return hsv;
}

static bool LAYER_COLUMNS(effect_params_t* params) {
    RGB_MATRIX_USE_LIMITS(led_min, led_max);

    hsv_t on_hsv = rgb_matrix_config.hsv;
    hsv_t off_hsv = rgb_matrix_config.hsv;
    off_hsv.s = 0;

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

    return rgb_matrix_check_finished_leds(led_max);
}

bool REACTIVE_WHITE(effect_params_t* params) {
    uint8_t active_layer = get_highest_layer(layer_state | default_layer_state);
    if (active_layer == 0) {
        return effect_runner_reactive(params, &REACTIVE_WHITE_math);
    }

    return LAYER_COLUMNS(params);
}

#        endif // RGB_MATRIX_CUSTOM_EFFECT_IMPLS
#endif         // RGB_MATRIX_KEYREACTIVE_ENABLED
