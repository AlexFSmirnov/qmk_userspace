#ifndef DRAW_LAYOUT_H
#define DRAW_LAYOUT_H

#include "qp.h"

// Function to draw a key label at the specified position
void draw_key_label(painter_device_t surface, int x, int y, const char* label);

// Function to draw a 3x5 grid layout
void draw_layout_grid(painter_device_t surface, int x, int y, bool mirrored, const char* keymap[5][6]);

#endif // DRAW_LAYOUT_H
