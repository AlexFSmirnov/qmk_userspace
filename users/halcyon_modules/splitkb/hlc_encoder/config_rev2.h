// Copyright 2026 splitkb.com (support@splitkb.com)
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#undef ENCODER_RESOLUTION

#if defined(KEYBOARD_splitkb_halcyon_ferris_rev1)
#define ENCODER_RESOLUTION 4
#else
#define ENCODER_RESOLUTIONS { 2, 4 }
#endif
