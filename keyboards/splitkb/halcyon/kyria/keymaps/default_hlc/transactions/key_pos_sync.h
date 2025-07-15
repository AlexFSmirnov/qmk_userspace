#include "quantum.h"

void key_pos_sync_slave_handler(uint8_t in_buflen, const void* in_data, uint8_t out_buflen, void* out_data);
void send_key_pos_to_slave(int row, int col);
void register_key_pos_sync_handler(void);
