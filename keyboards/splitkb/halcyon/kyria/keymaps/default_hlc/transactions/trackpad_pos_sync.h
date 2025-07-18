#pragma once

#include "quantum.h"

void trackpad_pos_sync_slave_handler(uint8_t in_buflen, const void* in_data, uint8_t out_buflen, void* out_data);
void send_trackpad_pos_to_slave(int x, int y);
void register_trackpad_pos_sync_handler(void);
