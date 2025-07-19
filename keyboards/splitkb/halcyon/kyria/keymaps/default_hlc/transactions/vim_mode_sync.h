#pragma once

#include "quantum.h"
#include "../qmk-vim/modes.h"

void vim_mode_sync_slave_handler(uint8_t in_buflen, const void* in_data, uint8_t out_buflen, void* out_data);
void send_vim_mode_to_slave(vim_mode_t mode, bool enabled);
void register_vim_mode_sync_handler(void);

// Functions to get synced vim mode state
vim_mode_t get_synced_vim_mode(void);
bool get_synced_vim_enabled(void);
bool is_synced_vim_mode_outdated(void);
