#include "macro_recorder.h"
#include "timer.h"
#include "eeprom.h"
#include <string.h>

// EEPROM layout
#define MACRO_EEPROM_MAGIC 0x4D41  // "MA" for Macro
#define MACRO_EEPROM_VERSION 1
// Use a safe address in user EEPROM space (starting at byte 1024)
#define MACRO_EEPROM_START 1024

typedef struct {
    uint16_t magic;
    uint8_t version;
    uint8_t slot;
    uint16_t event_count;
    bool has_content;
} macro_eeprom_header_t;

// Global state
static macro_recorder_state_t recorder_state = {0};

void macro_recorder_init(void) {
    memset(&recorder_state, 0, sizeof(macro_recorder_state_t));

    // Load all macros from EEPROM on init
    macro_recorder_load_all_from_eeprom();
}

void macro_recorder_start_recording(uint8_t slot) {
    if (slot >= MAX_MACRO_SLOTS) {
        return;
    }

    // Stop any existing recording or playback
    macro_recorder_stop_recording();
    macro_recorder_stop_playback();

    // Clear the target slot
    memset(&recorder_state.macros[slot], 0, sizeof(macro_t));

    // Start recording
    recorder_state.recording_active = true;
    recorder_state.current_slot = slot;
    recorder_state.macros[slot].is_recording = true;
    recorder_state.macros[slot].last_event_time = timer_read32();
}

void macro_recorder_stop_recording(void) {
    if (!recorder_state.recording_active) {
        return;
    }

    uint8_t slot = recorder_state.current_slot;
    recorder_state.macros[slot].is_recording = false;
    recorder_state.recording_active = false;

    // Mark as having content if any events were recorded
    if (recorder_state.macros[slot].event_count > 0) {
        recorder_state.macros[slot].has_content = true;
        // Save to EEPROM
        macro_recorder_save_to_eeprom(slot);
    }
}

void macro_recorder_play(uint8_t slot) {
    if (slot >= MAX_MACRO_SLOTS) {
        return;
    }

    if (!recorder_state.macros[slot].has_content) {
        return;
    }

    if (recorder_state.macros[slot].event_count == 0) {
        return;
    }

    // Don't restart if already playing this slot (prevents double trigger)
    if (recorder_state.playback_active && recorder_state.playback_slot == slot) {
        return;
    }

    // Stop any existing playback or recording
    macro_recorder_stop_playback();
    macro_recorder_stop_recording();

    // Start playback immediately (normal speed with delays)
    recorder_state.playback_active = true;
    recorder_state.playback_fast_mode = false;
    recorder_state.playback_slot = slot;
    recorder_state.playback_index = 0;
    recorder_state.playback_next_time = 0;  // Start immediately
}

void macro_recorder_play_fast(uint8_t slot) {
    if (slot >= MAX_MACRO_SLOTS) {
        return;
    }

    if (!recorder_state.macros[slot].has_content) {
        return;
    }

    if (recorder_state.macros[slot].event_count == 0) {
        return;
    }

    // Don't restart if already playing this slot (prevents double trigger)
    if (recorder_state.playback_active && recorder_state.playback_slot == slot) {
        return;
    }

    // Stop any existing playback or recording
    macro_recorder_stop_playback();
    macro_recorder_stop_recording();

    // Start playback immediately (fast mode without delays)
    recorder_state.playback_active = true;
    recorder_state.playback_fast_mode = true;
    recorder_state.playback_slot = slot;
    recorder_state.playback_index = 0;
    recorder_state.playback_next_time = 0;  // Start immediately
}

void macro_recorder_stop_playback(void) {
    recorder_state.playback_active = false;
    recorder_state.playback_index = 0;
}

void macro_recorder_clear(uint8_t slot) {
    if (slot >= MAX_MACRO_SLOTS) {
        return;
    }

    memset(&recorder_state.macros[slot], 0, sizeof(macro_t));

    // Clear from EEPROM as well
    macro_eeprom_header_t header = {
        .magic = MACRO_EEPROM_MAGIC,
        .version = MACRO_EEPROM_VERSION,
        .slot = slot,
        .event_count = 0,
        .has_content = false
    };

    uintptr_t addr = MACRO_EEPROM_START + (slot * (sizeof(macro_eeprom_header_t) + sizeof(macro_event_t) * MAX_MACRO_ACTIONS));
    eeprom_update_block(&header, (void*)addr, sizeof(macro_eeprom_header_t));
}

void macro_recorder_task(void) {
    if (!recorder_state.playback_active) {
        return;
    }

    uint8_t slot = recorder_state.playback_slot;
    macro_t* macro = &recorder_state.macros[slot];

    // Check if we've finished playback
    if (recorder_state.playback_index >= macro->event_count) {
        macro_recorder_stop_playback();
        return;
    }

    // Check if it's time to process the next event
    uint32_t now = timer_read32();
    if (recorder_state.playback_next_time != 0) {
        // Check if we've reached the target time
        // Use signed comparison to handle timer wraparound
        int32_t time_remaining = (int32_t)(recorder_state.playback_next_time - now);
        if (time_remaining > 0) {
            // Not time yet - still waiting
            return;
        }
    }

    // Get current event
    macro_event_t* event = &macro->events[recorder_state.playback_index];

    // Process the event
    switch (event->type) {
        case MACRO_EVENT_KEY_DOWN:
            // Set modifiers
            set_mods(event->data.key.mods);
            send_keyboard_report();  // Make sure mods are applied
            register_code16(event->data.key.keycode);
            break;

        case MACRO_EVENT_KEY_UP:
            unregister_code16(event->data.key.keycode);
            break;

        case MACRO_EVENT_ENCODER_CW:
            // Simulate encoder clockwise rotation by tapping the encoder keycode
            // This will be handled by encoder_update_user in the main code
            break;

        case MACRO_EVENT_ENCODER_CCW:
            // Simulate encoder counter-clockwise rotation
            break;

        case MACRO_EVENT_MOUSE_MOVE: {
            // Simulate mouse movement - apply the delta movement
            report_mouse_t mouse_report = pointing_device_get_report();
            mouse_report.x = event->data.mouse.x;
            mouse_report.y = event->data.mouse.y;
            pointing_device_set_report(mouse_report);
            pointing_device_send();
            break;
        }

        case MACRO_EVENT_DELAY:
            // In fast mode, skip delays completely
            if (recorder_state.playback_fast_mode) {
                recorder_state.playback_index++;
                recorder_state.playback_next_time = 0; // Process next event immediately
                return;
            }
            // In normal mode, schedule next event after the recorded delay
            recorder_state.playback_next_time = timer_read32() + event->data.delay.duration_ms;
            recorder_state.playback_index++;
            return;
    }

    // Move to next event
    recorder_state.playback_index++;

    // No default delay - we record all delays now!
    // Just trigger next event immediately (on next task call)
    recorder_state.playback_next_time = 0;
}

void macro_recorder_record_key_down(uint16_t keycode, uint8_t mods) {
    if (!recorder_state.recording_active) {
        return;
    }

    uint8_t slot = recorder_state.current_slot;
    macro_t* macro = &recorder_state.macros[slot];

    if (macro->event_count >= MAX_MACRO_ACTIONS) {
        return;  // Macro is full
    }

    // Calculate time since last event and add delay to preserve timing
    uint32_t current_time = timer_read32();
    uint32_t time_diff = TIMER_DIFF_32(current_time, macro->last_event_time);

    // ALWAYS record delays (even 0ms) to preserve exact timing
    // Only skip delay for the very first event
    if (macro->event_count > 0) {
        macro_event_t delay_event = {
            .type = MACRO_EVENT_DELAY,
            .data.delay.duration_ms = time_diff
        };
        macro->events[macro->event_count++] = delay_event;

        if (macro->event_count >= MAX_MACRO_ACTIONS) {
            return;
        }
    }

    // Record key down event
    macro_event_t event = {
        .type = MACRO_EVENT_KEY_DOWN,
        .data.key.keycode = keycode,
        .data.key.mods = mods
    };

    macro->events[macro->event_count++] = event;
    macro->last_event_time = current_time;
}

void macro_recorder_record_key_up(uint16_t keycode, uint8_t mods) {
    if (!recorder_state.recording_active) {
        return;
    }

    uint8_t slot = recorder_state.current_slot;
    macro_t* macro = &recorder_state.macros[slot];

    if (macro->event_count >= MAX_MACRO_ACTIONS) {
        return;
    }

    // Calculate time since last event and add delay to preserve timing
    uint32_t current_time = timer_read32();
    uint32_t time_diff = TIMER_DIFF_32(current_time, macro->last_event_time);

    // ALWAYS record delays (even 0ms) to preserve exact timing
    // Only skip delay for the very first event
    if (macro->event_count > 0) {
        macro_event_t delay_event = {
            .type = MACRO_EVENT_DELAY,
            .data.delay.duration_ms = time_diff
        };
        macro->events[macro->event_count++] = delay_event;

        if (macro->event_count >= MAX_MACRO_ACTIONS) {
            return;
        }
    }

    // Record key up event
    macro_event_t event = {
        .type = MACRO_EVENT_KEY_UP,
        .data.key.keycode = keycode,
        .data.key.mods = mods
    };

    macro->events[macro->event_count++] = event;
    macro->last_event_time = current_time;
}

void macro_recorder_record_encoder(uint8_t encoder_id, bool clockwise) {
    if (!recorder_state.recording_active) {
        return;
    }

    uint8_t slot = recorder_state.current_slot;
    macro_t* macro = &recorder_state.macros[slot];

    if (macro->event_count >= MAX_MACRO_ACTIONS) {
        return;
    }

    // Calculate time since last event and add delay to preserve timing
    uint32_t current_time = timer_read32();
    uint32_t time_diff = TIMER_DIFF_32(current_time, macro->last_event_time);

    // ALWAYS record delays (even 0ms) to preserve exact timing
    // Only skip delay for the very first event
    if (macro->event_count > 0) {
        macro_event_t delay_event = {
            .type = MACRO_EVENT_DELAY,
            .data.delay.duration_ms = time_diff
        };
        macro->events[macro->event_count++] = delay_event;

        if (macro->event_count >= MAX_MACRO_ACTIONS) {
            return;
        }
    }

    macro_event_t event = {
        .type = clockwise ? MACRO_EVENT_ENCODER_CW : MACRO_EVENT_ENCODER_CCW,
        .data.encoder.encoder_id = encoder_id
    };

    macro->events[macro->event_count++] = event;
    macro->last_event_time = current_time;
}

void macro_recorder_record_mouse_move(int8_t x, int8_t y) {
    if (!recorder_state.recording_active) {
        return;
    }

    uint8_t slot = recorder_state.current_slot;
    macro_t* macro = &recorder_state.macros[slot];

    if (macro->event_count >= MAX_MACRO_ACTIONS) {
        return;
    }

    uint32_t current_time = timer_read32();

    // Throttle mouse recording to reduce number of events
    // Only record if at least 30ms has passed since last mouse event
    static uint32_t last_mouse_time = 0;
    uint32_t time_since_last_mouse = TIMER_DIFF_32(current_time, last_mouse_time);

    if (time_since_last_mouse < 30) {
        // Too soon, skip this movement
        return;
    }

    last_mouse_time = current_time;

    // Calculate time since last event (of any type) and add delay to preserve timing
    uint32_t time_diff = TIMER_DIFF_32(current_time, macro->last_event_time);

    // ALWAYS record delays (even 0ms) to preserve exact timing
    // Only skip delay for the very first event
    if (macro->event_count > 0) {
        macro_event_t delay_event = {
            .type = MACRO_EVENT_DELAY,
            .data.delay.duration_ms = time_diff
        };
        macro->events[macro->event_count++] = delay_event;

        if (macro->event_count >= MAX_MACRO_ACTIONS) {
            return;
        }
    }

    macro_event_t event = {
        .type = MACRO_EVENT_MOUSE_MOVE,
        .data.mouse.x = x,
        .data.mouse.y = y
    };

    macro->events[macro->event_count++] = event;
    macro->last_event_time = current_time;
}

bool macro_recorder_is_recording(void) {
    return recorder_state.recording_active;
}

bool macro_recorder_is_playing(void) {
    return recorder_state.playback_active;
}

bool macro_recorder_has_content(uint8_t slot) {
    if (slot >= MAX_MACRO_SLOTS) {
        return false;
    }
    return recorder_state.macros[slot].has_content;
}

uint8_t macro_recorder_get_current_slot(void) {
    return recorder_state.current_slot;
}

uint16_t macro_recorder_get_recorded_count(void) {
    if (!recorder_state.recording_active) {
        return 0;
    }

    return recorder_state.macros[recorder_state.current_slot].event_count;
}

bool macro_recorder_slot_has_content(uint8_t slot) {
    if (slot >= MAX_MACRO_SLOTS) {
        return false;
    }

    return recorder_state.macros[slot].has_content;
}

void macro_recorder_save_to_eeprom(uint8_t slot) {
    if (slot >= MAX_MACRO_SLOTS) {
        return;
    }

    macro_t* macro = &recorder_state.macros[slot];

    // Prepare header
    macro_eeprom_header_t header = {
        .magic = MACRO_EEPROM_MAGIC,
        .version = MACRO_EEPROM_VERSION,
        .slot = slot,
        .event_count = macro->event_count,
        .has_content = macro->has_content
    };

    // Calculate EEPROM address for this slot
    uintptr_t addr = MACRO_EEPROM_START + (slot * (sizeof(macro_eeprom_header_t) + sizeof(macro_event_t) * MAX_MACRO_ACTIONS));

    // Write header
    eeprom_update_block(&header, (void*)addr, sizeof(macro_eeprom_header_t));

    // Write events
    addr += sizeof(macro_eeprom_header_t);
    eeprom_update_block(macro->events, (void*)addr, sizeof(macro_event_t) * macro->event_count);
}

void macro_recorder_load_from_eeprom(uint8_t slot) {
    if (slot >= MAX_MACRO_SLOTS) {
        return;
    }

    // Calculate EEPROM address for this slot
    uintptr_t addr = MACRO_EEPROM_START + (slot * (sizeof(macro_eeprom_header_t) + sizeof(macro_event_t) * MAX_MACRO_ACTIONS));

    // Read header
    macro_eeprom_header_t header;
    eeprom_read_block(&header, (void*)addr, sizeof(macro_eeprom_header_t));

    // Validate magic and version
    if (header.magic != MACRO_EEPROM_MAGIC || header.version != MACRO_EEPROM_VERSION) {
        return;  // Invalid data
    }

    // Validate slot matches
    if (header.slot != slot) {
        return;
    }

    // Clear the macro first
    memset(&recorder_state.macros[slot], 0, sizeof(macro_t));

    // Load event count and content flag
    recorder_state.macros[slot].event_count = header.event_count;
    recorder_state.macros[slot].has_content = header.has_content;

    // Read events if there's content
    if (header.has_content && header.event_count > 0) {
        addr += sizeof(macro_eeprom_header_t);
        uint16_t events_to_read = header.event_count < MAX_MACRO_ACTIONS ? header.event_count : MAX_MACRO_ACTIONS;
        eeprom_read_block(recorder_state.macros[slot].events, (void*)addr, sizeof(macro_event_t) * events_to_read);
    }
}

void macro_recorder_load_all_from_eeprom(void) {
    for (uint8_t i = 0; i < MAX_MACRO_SLOTS; i++) {
        macro_recorder_load_from_eeprom(i);
    }
}

