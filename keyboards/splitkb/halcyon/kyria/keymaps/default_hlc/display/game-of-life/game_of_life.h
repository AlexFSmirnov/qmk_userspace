#pragma once

#include "qp.h"
#include "quantum.h"

void game_of_life_step(void);
void draw_game_of_life_grid(painter_device_t surface);
bool process_game_of_life_display(painter_device_t surface);
void reset_game_of_life_grid(void);
void register_game_of_life_key_press(int row, int col);
