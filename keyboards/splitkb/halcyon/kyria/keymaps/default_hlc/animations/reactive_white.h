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

bool REACTIVE_WHITE(effect_params_t* params) {
    return effect_runner_reactive(params, &REACTIVE_WHITE_math);
}

#        endif // RGB_MATRIX_CUSTOM_EFFECT_IMPLS
#endif         // RGB_MATRIX_KEYREACTIVE_ENABLED

