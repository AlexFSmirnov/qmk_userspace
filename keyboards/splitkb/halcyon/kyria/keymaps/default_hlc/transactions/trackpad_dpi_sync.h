#pragma once

#include "quantum.h"

void register_trackpad_dpi_sync_handler(void);
void send_trackpad_dpi_to_slave(uint16_t dpi);
uint16_t get_synced_trackpad_dpi(void);
void save_trackpad_dpi_to_eeprom(uint16_t dpi);
uint16_t load_trackpad_dpi_from_eeprom(void);
void init_trackpad_dpi(void);
bool increase_trackpad_dpi(void);
bool decrease_trackpad_dpi(void);
