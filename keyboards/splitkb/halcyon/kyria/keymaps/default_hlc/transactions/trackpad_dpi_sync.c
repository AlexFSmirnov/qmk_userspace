#include "quantum.h"
#include "pointing_device.h"
#include "transactions.h"
#include "trackpad_dpi_sync.h"
#include "eeprom.h"
#include "../display/notification/notification.h"

// EEPROM storage for DPI settings
#define EEPROM_DPI_ADDR (0x100)  // User area in EEPROM
#define DPI_MAGIC 0x4450  // "DP" for DPI
#define DPI_STEP 50
#define DPI_MIN 100
#define DPI_MAX 1000

typedef struct {
    uint16_t magic;
    uint16_t dpi;
} dpi_eeprom_t;

typedef struct _trackpad_dpi_master_to_slave_t {
    uint16_t dpi;
} trackpad_dpi_master_to_slave_t;

static uint16_t slave_trackpad_dpi = TRACKPAD_DEFAULT_CPI;
static uint16_t current_trackpad_dpi = TRACKPAD_DEFAULT_CPI;

void trackpad_dpi_sync_slave_handler(uint8_t in_buflen, const void* in_data, uint8_t out_buflen, void* out_data) {
    const trackpad_dpi_master_to_slave_t *m2s = (const trackpad_dpi_master_to_slave_t*)in_data;
    slave_trackpad_dpi = m2s->dpi;

    // Apply the DPI change on the slave side
    #ifdef HLC_CIRQUE_TRACKPAD
    pointing_device_set_cpi(slave_trackpad_dpi);
    #endif
}

void register_trackpad_dpi_sync_handler(void) {
    transaction_register_rpc(TRACKPAD_DPI_SYNC, trackpad_dpi_sync_slave_handler);
}

void send_trackpad_dpi_to_slave(uint16_t dpi) {
    if (!is_keyboard_master()) {
        return;
    }

    trackpad_dpi_master_to_slave_t m2s = {dpi};
    transaction_rpc_send(TRACKPAD_DPI_SYNC, sizeof(m2s), &m2s);
}

uint16_t get_synced_trackpad_dpi(void) {
    if (is_keyboard_master()) {
        return current_trackpad_dpi;
    }
    return slave_trackpad_dpi;
}

void save_trackpad_dpi_to_eeprom(uint16_t dpi) {
    dpi_eeprom_t data = {
        .magic = DPI_MAGIC,
        .dpi = dpi
    };
    eeprom_update_block((const void*)&data, (void*)((uintptr_t)EEPROM_DPI_ADDR), sizeof(data));
}

uint16_t load_trackpad_dpi_from_eeprom(void) {
    dpi_eeprom_t data;
    eeprom_read_block((void*)&data, (const void*)((uintptr_t)EEPROM_DPI_ADDR), sizeof(data));

    // Validate magic number and DPI value
    if (data.magic != DPI_MAGIC || data.dpi < DPI_MIN || data.dpi > DPI_MAX) {
        // EEPROM not initialized or corrupted, use default
        uint16_t default_dpi = 550;
        save_trackpad_dpi_to_eeprom(default_dpi);  // Initialize EEPROM
        return default_dpi;
    }

    return data.dpi;
}

static bool dpi_synced_to_slave = false;

void init_trackpad_dpi(void) {
    if (is_keyboard_master()) {
        current_trackpad_dpi = load_trackpad_dpi_from_eeprom();

        #ifdef HLC_CIRQUE_TRACKPAD
        pointing_device_set_cpi(current_trackpad_dpi);
        #endif

        // Mark that we need to sync to slave
        dpi_synced_to_slave = false;

        // Show current DPI on startup
        char dpi_message[16];
        snprintf(dpi_message, sizeof(dpi_message), "DPI: %d", current_trackpad_dpi);
        send_notification(dpi_message, 2000); // Show for 2 seconds on startup
    }
}

// Call this from housekeeping to ensure slave gets the DPI after init
void trackpad_dpi_sync_task(void) {
    if (!is_keyboard_master()) {
        return;
    }

    // Keep syncing until successful (or for first few seconds)
    static uint16_t sync_attempts = 0;
    if (!dpi_synced_to_slave && sync_attempts < 100) {  // Try for ~1 second
        send_trackpad_dpi_to_slave(current_trackpad_dpi);
        sync_attempts++;
        if (sync_attempts >= 10) {  // After 10 attempts, consider it done
            dpi_synced_to_slave = true;
        }
    }
}

bool increase_trackpad_dpi(void) {
    uint16_t new_dpi = current_trackpad_dpi + DPI_STEP;
    if (new_dpi <= DPI_MAX) {
        current_trackpad_dpi = new_dpi;

        #ifdef HLC_CIRQUE_TRACKPAD
        pointing_device_set_cpi(current_trackpad_dpi);
        #endif

        save_trackpad_dpi_to_eeprom(current_trackpad_dpi);
        send_trackpad_dpi_to_slave(current_trackpad_dpi);

        // Show notification
        char dpi_message[16];
        snprintf(dpi_message, sizeof(dpi_message), "DPI: %d", current_trackpad_dpi);
        send_notification(dpi_message, 1500); // Show for 1.5 seconds

        return true;
    }
    return false;
}

bool decrease_trackpad_dpi(void) {
    uint16_t new_dpi = current_trackpad_dpi - DPI_STEP;
    if (new_dpi >= DPI_MIN) {
        current_trackpad_dpi = new_dpi;

        #ifdef HLC_CIRQUE_TRACKPAD
        pointing_device_set_cpi(current_trackpad_dpi);
        #endif

        save_trackpad_dpi_to_eeprom(current_trackpad_dpi);
        send_trackpad_dpi_to_slave(current_trackpad_dpi);

        // Show notification
        char dpi_message[16];
        snprintf(dpi_message, sizeof(dpi_message), "DPI: %d", current_trackpad_dpi);
        send_notification(dpi_message, 1500); // Show for 1.5 seconds

        return true;
    }
    return false;
}
