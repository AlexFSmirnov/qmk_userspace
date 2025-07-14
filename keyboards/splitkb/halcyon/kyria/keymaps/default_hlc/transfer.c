#include "quantum.h"
#include "transactions.h"

// This code works but is not used yet

typedef struct _master_to_slave_t {
    int m2s_data;
} master_to_slave_t;

typedef struct _slave_to_master_t {
    int s2m_data;
} slave_to_master_t;

void user_sync_a_slave_handler(uint8_t in_buflen, const void* in_data, uint8_t out_buflen, void* out_data) {
    const master_to_slave_t *m2s = (const master_to_slave_t*)in_data;
    slave_to_master_t *s2m = (slave_to_master_t*)out_data;
    s2m->s2m_data = m2s->m2s_data + 5; // whatever comes in, add 5 so it can be sent back
}

void keyboard_post_init_user(void) {
    transaction_register_rpc(USER_SYNC_A, user_sync_a_slave_handler);
}

void housekeeping_task_user(void) {
    if (is_keyboard_master()) {
        // Interact with slave every 500ms
        static uint32_t last_sync = 0;
        if (timer_elapsed32(last_sync) > 500) {
            master_to_slave_t m2s = {2};
            slave_to_master_t s2m = {0};
            if(transaction_rpc_exec(USER_SYNC_A, sizeof(m2s), &m2s, sizeof(s2m), &s2m)) {
                uprintf("Slave value: %d\n", s2m.s2m_data); // this will now be 11, as the slave adds 5
            } else {
                uprint("Slave sync failed!\n");
            }
            last_sync = timer_read32();
        }
    }
}
