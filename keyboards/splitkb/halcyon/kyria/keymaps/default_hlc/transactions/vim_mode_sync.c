#include "keyboard.h"
#include "quantum.h"
#include "transactions.h"
#include "vim_mode_sync.h"
#include "../qmk-vim/modes.h"
#include "../qmk-vim/vim.h"

typedef struct _vim_mode_master_to_slave_t {
    vim_mode_t mode;
    bool enabled;
} vim_mode_master_to_slave_t;

static vim_mode_t slave_vim_mode = NORMAL_MODE;
static bool slave_vim_enabled = false;

void vim_mode_sync_slave_handler(uint8_t in_buflen, const void* in_data, uint8_t out_buflen, void* out_data) {
    const vim_mode_master_to_slave_t *m2s = (const vim_mode_master_to_slave_t*)in_data;
    slave_vim_mode = m2s->mode;
    slave_vim_enabled = m2s->enabled;
}

void register_vim_mode_sync_handler(void) {
    transaction_register_rpc(VIM_MODE_SYNC, vim_mode_sync_slave_handler);
}

void send_vim_mode_to_slave(vim_mode_t mode, bool enabled) {
    if (!is_keyboard_master()) {
        return;
    }

    vim_mode_master_to_slave_t m2s = {mode, enabled};
    transaction_rpc_send(VIM_MODE_SYNC, sizeof(m2s), &m2s);
}

// Functions to get vim mode state on slave
vim_mode_t get_synced_vim_mode(void) {
    if (is_keyboard_master()) {
        return get_vim_mode();
    }
    return slave_vim_mode;
}

bool get_synced_vim_enabled(void) {
    if (is_keyboard_master()) {
        return vim_mode_enabled();
    }
    return slave_vim_enabled;
}

bool is_synced_vim_mode_outdated(void) {
    if (!is_keyboard_master()) {
        return false;
    }

    return get_vim_mode() != slave_vim_mode || vim_mode_enabled() != slave_vim_enabled;
}
