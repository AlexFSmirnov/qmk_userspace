ENCODER_MAP_ENABLE = yes

# This adds module functionality to your keyboard (files found in users/halcyon_modules)
USER_NAME := halcyon_modules

CONSOLE_ENABLE = yes

SRC += user.c

SRC += display/controller.c
SRC += display/keymap/keycode_strings.c
SRC += display/keymap/draw_keymap.c
SRC += display/keymap/process_keymap.c

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
