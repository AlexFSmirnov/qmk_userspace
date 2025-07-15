#include "quantum.h"
#include "transactions.h"
#include "key_pos_sync.h"
#include "display/game-of-life/game_of_life.h"

typedef struct _master_to_slave_t {
    int row;
    int col;
} master_to_slave_t;

void key_pos_sync_slave_handler(uint8_t in_buflen, const void* in_data, uint8_t out_buflen, void* out_data) {
    const master_to_slave_t *m2s = (const master_to_slave_t*)in_data;
    register_game_of_life_key_press(m2s->row, m2s->col);
}

void register_key_pos_sync_handler(void) {
    transaction_register_rpc(KEY_POS_SYNC, key_pos_sync_slave_handler);
}

void send_key_pos_to_slave(int row, int col) {
    if (!is_keyboard_master()) {
        return;
    }

    master_to_slave_t m2s = {row, col};
    transaction_rpc_send(KEY_POS_SYNC, sizeof(m2s), &m2s);
}
