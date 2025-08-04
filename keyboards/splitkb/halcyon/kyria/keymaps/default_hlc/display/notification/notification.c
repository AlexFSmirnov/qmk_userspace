#include "notification.h"
#include "../fonts/9x12.qff.h"
#include "timer.h"
#include <string.h>
#include <stdio.h>

#define LCD_WIDTH 135
#define LCD_HEIGHT 240
#define MAX_NOTIFICATION_LENGTH 32

// Notification state
static char notification_text[MAX_NOTIFICATION_LENGTH] = {0};
static uint32_t notification_start_time = 0;
static uint32_t notification_duration = 0;
static bool notification_active = false;
static painter_font_handle_t font_9x12_handle = NULL;

void send_notification(const char* message, uint32_t duration_ms) {
    if (!message || strlen(message) == 0) {
        return;
    }

    // Copy message to notification buffer
    strncpy(notification_text, message, MAX_NOTIFICATION_LENGTH - 1);
    notification_text[MAX_NOTIFICATION_LENGTH - 1] = '\0';

    // Set timing
    notification_start_time = timer_read32();
    notification_duration = duration_ms;
    notification_active = true;
}

void clear_notification(void) {
    notification_active = false;
    notification_text[0] = '\0';
}

bool process_notification_display(painter_device_t surface) {
    if (!notification_active) {
        return false;
    }

    // Check if notification has expired
    uint32_t current_time = timer_read32();
    if (TIMER_DIFF_32(current_time, notification_start_time) >= notification_duration) {
        clear_notification();
        return true; // Return true to trigger redraw to clear the notification
    }

    // Load font if not already loaded
    if (font_9x12_handle == NULL) {
        font_9x12_handle = qp_load_font_mem(font_9x12);
        if (font_9x12_handle == NULL) {
            return false;
        }
    }

    // Calculate text dimensions
    int16_t text_width = qp_textwidth(font_9x12_handle, notification_text);
    int16_t text_height = font_9x12_handle->line_height;

    // Calculate position for center of screen
    int text_x = (LCD_WIDTH - text_width) / 2;
    int text_y = (LCD_HEIGHT - text_height) / 2;

    // Define background rectangle with padding
    int padding = 4;
    int bg_x = text_x - padding;
    int bg_y = text_y - padding;
    int bg_width = text_width + (padding * 2);
    int bg_height = text_height + (padding * 2);

    // Ensure background stays within screen bounds
    if (bg_x < 0) bg_x = 0;
    if (bg_y < 0) bg_y = 0;
    if (bg_x + bg_width > LCD_WIDTH) bg_width = LCD_WIDTH - bg_x;
    if (bg_y + bg_height > LCD_HEIGHT) bg_height = LCD_HEIGHT - bg_y;

    // Draw background rectangle (black with slight transparency effect)
    qp_rect(surface, bg_x, bg_y, bg_x + bg_width - 1, bg_y + bg_height - 1, HSV_BLACK, true);

    // Draw border around notification
    qp_rect(surface, bg_x, bg_y, bg_x + bg_width - 1, bg_y + bg_height - 1, HSV_WHITE, false);

    // Draw the notification text in white
    qp_drawtext_recolor(surface, text_x, text_y, font_9x12_handle, notification_text, 0, 0, 255, HSV_BLACK);

    return true; // Return true to indicate something was drawn
}
