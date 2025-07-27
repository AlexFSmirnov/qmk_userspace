#pragma once

#include "quantum.h"

void trackpad_shift_sync_slave_handler(uint8_t in_buflen, const void* in_data, uint8_t out_buflen, void* out_data);
void send_trackpad_shift_to_slave(bool is_shifted);
void register_trackpad_shift_sync_handler(void);

// Functions to get synced trackpad shift state
bool get_synced_trackpad_shifted(void);
