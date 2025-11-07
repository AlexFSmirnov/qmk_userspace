#pragma once

#include "quantum.h"
#include <stdint.h>

// Maximum number of macro slots
#define MAX_MACRO_SLOTS 10

// Maximum number of actions per macro
#define MAX_MACRO_ACTIONS 128

// Macro event types
typedef enum {
    MACRO_EVENT_KEY_DOWN,
    MACRO_EVENT_KEY_UP,
    MACRO_EVENT_ENCODER_CW,
    MACRO_EVENT_ENCODER_CCW,
    MACRO_EVENT_MOUSE_MOVE,
    MACRO_EVENT_DELAY,
} macro_event_type_t;

// Macro event structure
typedef struct {
    macro_event_type_t type;
    union {
        struct {
            uint16_t keycode;
            uint8_t mods;  // Active modifiers at time of press
        } key;
        struct {
            uint8_t encoder_id;
        } encoder;
        struct {
            int8_t x;
            int8_t y;
        } mouse;
        struct {
            uint16_t duration_ms;
        } delay;
    } data;
} macro_event_t;

// Macro structure
typedef struct {
    bool is_recording;
    bool has_content;
    uint16_t event_count;
    macro_event_t events[MAX_MACRO_ACTIONS];
    uint32_t last_event_time;
} macro_t;

// Macro recorder state
typedef struct {
    bool recording_active;
    uint8_t current_slot;  // Which slot we're recording to
    uint8_t playback_slot; // Which slot we're playing back
    bool playback_active;
    uint16_t playback_index;
    uint32_t playback_next_time;
    macro_t macros[MAX_MACRO_SLOTS];
} macro_recorder_state_t;

// Function declarations
void macro_recorder_init(void);
void macro_recorder_start_recording(uint8_t slot);
void macro_recorder_stop_recording(void);
void macro_recorder_play(uint8_t slot);
void macro_recorder_stop_playback(void);
void macro_recorder_clear(uint8_t slot);
void macro_recorder_task(void);

// Event recording functions
void macro_recorder_record_key_down(uint16_t keycode, uint8_t mods);
void macro_recorder_record_key_up(uint16_t keycode, uint8_t mods);
void macro_recorder_record_encoder(uint8_t encoder_id, bool clockwise);
void macro_recorder_record_mouse_move(int8_t x, int8_t y);

// State query functions
bool macro_recorder_is_recording(void);
bool macro_recorder_is_playing(void);
uint8_t macro_recorder_get_current_slot(void);
uint16_t macro_recorder_get_recorded_count(void);
bool macro_recorder_slot_has_content(uint8_t slot);

// EEPROM functions
void macro_recorder_save_to_eeprom(uint8_t slot);
void macro_recorder_load_from_eeprom(uint8_t slot);
void macro_recorder_load_all_from_eeprom(void);

