#pragma once

#include "qp.h"

// Function declarations for keymap drawing functionality
void draw_key_label(painter_device_t surface, int x, int y, int row, int col, const char* label);
void draw_key(painter_device_t surface, int x, int y, int row, int col, const char* label);
void draw_layout_grid(painter_device_t surface, int x, int y, bool mirrored, const char* keymap[5][6]);
