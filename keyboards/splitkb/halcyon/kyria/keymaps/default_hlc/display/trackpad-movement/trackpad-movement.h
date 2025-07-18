#pragma once

#include "qp.h"
#include "quantum.h"

bool process_trackpad_movement_display(painter_device_t surface);
void clear_trackpad_movement(void);
void register_trackpad_movement(int x, int y);
