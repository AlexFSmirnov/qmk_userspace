#pragma once

#include "qp.h"

void send_notification(const char* message, uint32_t duration_ms);
bool process_notification_display(painter_device_t surface);
void clear_notification(void);
