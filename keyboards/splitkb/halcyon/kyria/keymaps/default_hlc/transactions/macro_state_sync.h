#pragma once

#include "quantum.h"

// Sync macro recording state to slave half
void send_macro_state_to_slave(bool is_recording, uint8_t slot, uint16_t action_count);

// Register the transaction handler
void register_macro_state_sync_handler(void);

// Get synced macro state
bool get_synced_macro_recording_state(void);
uint8_t get_synced_macro_slot(void);
uint16_t get_synced_macro_action_count(void);

