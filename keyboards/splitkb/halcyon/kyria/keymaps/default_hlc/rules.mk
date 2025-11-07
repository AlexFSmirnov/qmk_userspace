ENCODER_MAP_ENABLE = yes

# This adds module functionality to your keyboard (files found in users/halcyon_modules)
USER_NAME := halcyon_modules

CONSOLE_ENABLE = yes

SRC += user.c

SRC += display/controller.c
SRC += display/utils.c
SRC += display/notification/notification.c
SRC += display/keymap/keycode_strings.c
SRC += display/keymap/draw_keymap.c
SRC += display/keymap/process_keymap.c
SRC += display/game-of-life/game_of_life.c
SRC += display/trackpad-movement/trackpad-movement.c
SRC += display/vim-header/vim-header.c

SRC += display/fonts/3x5.qff.c
SRC += display/fonts/4x7.qff.c
SRC += display/fonts/5x7.qff.c
SRC += display/fonts/9x12.qff.c
SRC += display/fonts/12x20.qff.c

SRC += display/images/icons/backspace.qgf.c
SRC += display/images/icons/tab.qgf.c
SRC += display/images/icons/space.qgf.c
SRC += display/images/icons/shift.qgf.c
SRC += display/images/icons/enter.qgf.c
SRC += display/images/icons/lock.qgf.c
SRC += display/images/icons/mute.qgf.c
SRC += display/images/icons/volume_down.qgf.c
SRC += display/images/icons/volume_up.qgf.c

SRC += qmk-vim/mac_mode.c
SRC += qmk-vim/process_func.c
SRC += qmk-vim/numbered_actions.c
SRC += qmk-vim/motions.c
SRC += qmk-vim/actions.c
SRC += qmk-vim/modes.c
SRC += qmk-vim/vim.c

SRC += transactions/key_pos_sync.c
SRC += transactions/trackpad_pos_sync.c
SRC += transactions/vim_mode_sync.c
SRC += transactions/trackpad_shift_sync.c
SRC += transactions/trackpad_dpi_sync.c
SRC += transactions/macro_state_sync.c

SRC += macros/macro_recorder.c
SRC += display/macro-status/macro-status.c

RGB_MATRIX_CUSTOM_USER = yes
