#include "quantum.h"
#include "pointing_device.h"
#include "transactions.h"
#include "trackpad_shift_sync.h"

typedef struct _trackpad_shift_master_to_slave_t {
    bool is_shifted;
} trackpad_shift_master_to_slave_t;

static bool slave_trackpad_shifted = false;

void trackpad_shift_sync_slave_handler(uint8_t in_buflen, const void* in_data, uint8_t out_buflen, void* out_data) {
    const trackpad_shift_master_to_slave_t *m2s = (const trackpad_shift_master_to_slave_t*)in_data;
    slave_trackpad_shifted = m2s->is_shifted;

    // Apply the CPI change on the slave side
    #ifdef HLC_CIRQUE_TRACKPAD
    if (slave_trackpad_shifted) {
        pointing_device_set_cpi(TRACKPAD_SHIFT_CPI);
    } else {
        pointing_device_set_cpi(TRACKPAD_DEFAULT_CPI);
    }
    #endif
}

void register_trackpad_shift_sync_handler(void) {
    transaction_register_rpc(TRACKPAD_SHIFT_SYNC, trackpad_shift_sync_slave_handler);
}

void send_trackpad_shift_to_slave(bool is_shifted) {
    if (!is_keyboard_master()) {
        return;
    }

    trackpad_shift_master_to_slave_t m2s = {is_shifted};
    transaction_rpc_send(TRACKPAD_SHIFT_SYNC, sizeof(m2s), &m2s);
}

// Functions to get trackpad shift state on slave
bool get_synced_trackpad_shifted(void) {
    if (is_keyboard_master()) {
        // On master, we need to track the state ourselves
        // This could be enhanced by storing the master state
        return false; // Placeholder - the master will handle this directly
    }
    return slave_trackpad_shifted;
}
