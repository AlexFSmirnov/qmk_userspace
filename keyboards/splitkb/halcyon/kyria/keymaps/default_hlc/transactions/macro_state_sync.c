#include "macro_state_sync.h"
#include "transactions.h"

typedef struct {
    bool is_recording;
    uint8_t slot;
    uint16_t action_count;
} macro_state_sync_t;

static macro_state_sync_t synced_macro_state = {false, 0, 0};

void send_macro_state_to_slave(bool is_recording, uint8_t slot, uint16_t action_count) {
    macro_state_sync_t data = {
        .is_recording = is_recording,
        .slot = slot,
        .action_count = action_count
    };

    transaction_rpc_send(MACRO_STATE_SYNC, sizeof(data), &data);
}

static void macro_state_sync_slave_handler(uint8_t initiator2slave_buffer_size, const void* initiator2slave_buffer, uint8_t slave2initiator_buffer_size, void* slave2initiator_buffer) {
    const macro_state_sync_t* data = (const macro_state_sync_t*)initiator2slave_buffer;
    synced_macro_state = *data;
}

void register_macro_state_sync_handler(void) {
    transaction_register_rpc(MACRO_STATE_SYNC, macro_state_sync_slave_handler);
}

bool get_synced_macro_recording_state(void) {
    return synced_macro_state.is_recording;
}

uint8_t get_synced_macro_slot(void) {
    return synced_macro_state.slot;
}

uint16_t get_synced_macro_action_count(void) {
    return synced_macro_state.action_count;
}

