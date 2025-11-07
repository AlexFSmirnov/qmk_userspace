# Dynamic Macro Recording System

A powerful, vim-inspired macro recording system for QMK that allows you to record and replay complex sequences of keyboard inputs, including keys, modifiers, layer shifts, encoder movements, and trackpad gestures.

## Features

### Recording Capabilities
- **Complete Input Recording**: Records all hardware inputs including:
  - Key presses and releases with modifier states
  - Layer shifts
  - Encoder rotations (clockwise and counter-clockwise)
  - Trackpad/mouse movements
  - Timing delays between actions

### Visual Feedback
- **RGB Lighting Indicators**:
  - Keys with recorded macros light up in white on the macro layer
  - Currently recording slot shows in red
  - Normal layer colors for empty macro slots

- **Display Status**:
  - Real-time recording indicator showing "REC M{slot}: {action_count}"
  - Red background during recording for high visibility
  - Macro indicators in keymap preview display
  - White highlighting for keys with assigned macros

### Persistent Storage
- **EEPROM Storage**: Macros are automatically saved to EEPROM and persist across power cycles
- **10 Macro Slots**: Store up to 10 different macros
- **128 Actions per Macro**: Each macro can contain up to 128 recorded actions

### Split Keyboard Support
- **Transaction Sync**: Recording state synced between split keyboard halves
- **Dual Display Support**: Recording status appears on both displays

## Usage

### Accessing the Macro Layer
1. Press the `HUB` layer key (usually on thumb cluster)
2. Then press the `MACROS` key to enter the macro layer

### Recording a Macro
1. On the macro layer, press one of the REC keys (REC1-REC10 on the **top row**)
2. The display will show "REC M{slot}: 0" and the key will turn red
3. Perform your desired sequence of actions (keypresses, mouse movements, encoder scrolling)
4. The action counter on the display increments as you record
5. Press the same REC key again (or the STOP key) to finish recording
6. The macro is automatically saved to EEPROM

### Playing Back a Macro
1. On the macro layer, press one of the PLAY keys (M1-M10 on the **middle row**)
2. The macro will play back with the same timing as recorded
3. Press STOP to interrupt playback if needed
4. Pressing a PLAY key will stop any currently playing macro first

### Clearing a Macro
1. On the macro layer, press one of the CLEAR keys (CLR1-CLR10 on the **bottom row**)
2. The macro is immediately deleted from that slot and EEPROM
3. Clear keys glow red when they have content to delete

### Macro Layer Layout (LEFT SIDE ONLY)

```
Top Row:    [  ] [REC1][REC2][REC3][REC4][REC5]
Middle Row: [  ] [M1  ][M2  ][M3  ][M4  ][M5  ]
               PLAY  PLAY  PLAY  PLAY  PLAY
Bottom Row: [SFT][CLR1][CLR2][CLR3][CLR4][CLR5]
Thumbs:          [  ] [  ] [  ] [STOP]
```

- **Top Row**: Record macros (toggle recording on/off)
- **Middle Row**: Play macros
- **Bottom Row**: Clear macros (with Shift for hand position)
- **Thumbs**: Stop recording/playback

**Note:** Only 5 macro slots (M1-M5) are available, all on the left side for easy one-handed access

### Visual Indicators

**REC Keys (Top Row):**
- **Green**: Empty slot, ready to record new macro
- **Yellow**: Has existing macro (will overwrite if pressed)
- **Red**: Currently recording this slot

**PLAY Keys (Middle Row):**
- **White**: Macro exists and ready to play
- **Normal color**: No macro in this slot

**CLEAR Keys (Bottom Row):**
- **Red**: Has content that can be deleted
- **Normal color**: No macro to clear

## Implementation Details

### File Structure
```
macros/
├── macro_recorder.h         - API definitions and data structures
├── macro_recorder.c         - Core recording/playback logic
└── README.md                - This file

display/macro-status/
├── macro-status.h           - Display integration header
└── macro-status.c           - Recording status display

transactions/
├── macro_state_sync.h       - Split keyboard sync header
└── macro_state_sync.c       - Split keyboard sync implementation
```

### Key Components

#### Data Structures
- `macro_event_t`: Individual recorded action (key, encoder, mouse, delay)
- `macro_t`: Complete macro with up to 128 events
- `macro_recorder_state_t`: Global recorder state management

#### Event Types
- `MACRO_EVENT_KEY_DOWN`: Key press with modifiers
- `MACRO_EVENT_KEY_UP`: Key release
- `MACRO_EVENT_ENCODER_CW`: Encoder clockwise rotation
- `MACRO_EVENT_ENCODER_CCW`: Encoder counter-clockwise rotation
- `MACRO_EVENT_MOUSE_MOVE`: Trackpad/mouse movement
- `MACRO_EVENT_DELAY`: Timing delay between actions

#### Integration Points
- **user.c**: Main integration with process_record_user, pointing_device_task_user, encoder_update_user
- **controller.c**: Display integration for recording status
- **reactive_white.h**: RGB lighting effects for macro layer
- **draw_keymap.c**: Keymap preview with macro indicators

### Memory Usage
Each macro slot uses approximately:
- Header: 8 bytes
- Events: 128 events × 8 bytes = 1024 bytes
- **Total per slot**: ~1032 bytes
- **Total for 10 slots**: ~10KB EEPROM

### Timing
- Delays > 50ms between actions are recorded
- Playback maintains recorded timing
- Minimum 10ms delay between non-delay events

## Technical Notes

### EEPROM Address Space
- Macros stored starting at address 1024 (safe user space)
- Magic number: 0x4D41 ("MA")
- Version: 1

### Limitations
- Maximum 128 actions per macro
- Maximum 10 macro slots
- Mouse movement recorded as discrete events (not continuous)
- Encoder events recorded but playback may vary by implementation

### Future Enhancements
- Macro editing/trimming
- Macro chaining
- Conditional playback
- Macro export/import
- Variable playback speed

