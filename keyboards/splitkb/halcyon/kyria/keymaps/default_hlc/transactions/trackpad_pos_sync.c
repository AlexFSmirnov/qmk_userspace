#include "quantum.h"
#include "transactions.h"
#include "trackpad_pos_sync.h"
#include "display/trackpad-movement/trackpad-movement.h"

typedef struct _trackpad_master_to_slave_t {
    int x;
    int y;
} trackpad_master_to_slave_t;

void trackpad_pos_sync_slave_handler(uint8_t in_buflen, const void* in_data, uint8_t out_buflen, void* out_data) {
    const trackpad_master_to_slave_t *m2s = (const trackpad_master_to_slave_t*)in_data;
    register_trackpad_movement(m2s->x, m2s->y);
}

void register_trackpad_pos_sync_handler(void) {
    transaction_register_rpc(TRACKPAD_POS_SYNC, trackpad_pos_sync_slave_handler);
}

void send_trackpad_pos_to_slave(int x, int y) {
    if (!is_keyboard_master()) {
        return;
    }

    trackpad_master_to_slave_t m2s = {x, y};
    transaction_rpc_send(TRACKPAD_POS_SYNC, sizeof(m2s), &m2s);
}
