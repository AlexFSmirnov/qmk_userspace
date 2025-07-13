ENCODER_MAP_ENABLE = yes

# This adds module functionality to your keyboard (files found in users/halcyon_modules)
USER_NAME := halcyon_modules

CONSOLE_ENABLE = yes

SRC += user.c
SRC += fonts/3x5.qff.c
SRC += fonts/4x7.qff.c
SRC += fonts/5x7.qff.c
SRC += fonts/9x12.qff.c
SRC += fonts/12x20.qff.c
SRC += display/draw_layout.c
