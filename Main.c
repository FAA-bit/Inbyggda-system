#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>
#include <string.h>

#include "lcd.h"
#include "Ads.h"

/* ------------------------------------------------------------------ */
/*  Configuration                                                       */
/* ------------------------------------------------------------------ */

#define F_CPU            16000000UL
#define DISPLAY_DURATION_MS   20000UL  /* 20 seconds per ad */
#define SCROLL_DELAY_MS         300UL  /* Speed of scroll */
#define BLINK_ON_MS             600UL
#define BLINK_OFF_MS            400UL
#define LCD_COLS                   16

/* ------------------------------------------------------------------ */
/*  Simple millisecond counter using Timer1                             */
/* ------------------------------------------------------------------ */

volatile uint32_t ms_ticks = 0;

/* Call in main loop - not interrupt-driven to keep it simple */
static void delay_ms_counted(uint16_t ms) {
    for (uint16_t i = 0; i < ms; i++) {
        _delay_ms(1);
        ms_ticks++;
    }
}

static uint32_t uptime_minutes(void) {
    return ms_ticks / 60000UL;
}

/* ------------------------------------------------------------------ */
/*  Display helpers                                                     */
/* ------------------------------------------------------------------ */

/* Centre a string on a 16-char row */
static void print_centred(const char *str, uint8_t row) {
    uint8_t len = (uint8_t)strlen(str);
    uint8_t col = (len < LCD_COLS) ? (LCD_COLS - len) / 2 : 0;
    lcd_set_cursor(col, row);
    lcd_print(str);
}

/* Show static ad for DISPLAY_DURATION_MS */
static void show_static(const Ad *ad) {
    lcd_clear();
    print_centred(ad->line1, 0);
    if (ad->line2) print_centred(ad->line2, 1);
    delay_ms_counted(DISPLAY_DURATION_MS);
}

/* Scroll line1 across row 0, show line2 static on row 1 */
static void show_scroll(const Ad *ad) {
    uint8_t  text_len = (uint8_t)strlen(ad->line1);
    /* Pad with spaces so text enters from right and exits to left */
    /* Total travel = LCD_COLS + text_len steps */
    uint16_t total_steps = LCD_COLS + text_len;
    uint32_t elapsed = 0;

    lcd_clear();
    if (ad->line2) print_centred(ad->line2, 1);

    char buf[LCD_COLS + 1];

    while (elapsed < DISPLAY_DURATION_MS) {
        for (uint8_t step = 0; step < total_steps; step++) {
            /* Build the 16-char window to display */
            for (uint8_t col = 0; col < LCD_COLS; col++) {
                /* Position in the virtual string (spaces + text + spaces) */
                int16_t src = (int16_t)step - (int16_t)(LCD_COLS - 1) + (int16_t)col;
                if (src < 0 || src >= text_len)
                    buf[col] = ' ';
                else
                    buf[col] = ad->line1[src];
            }
            buf[LCD_COLS] = '\0';
            lcd_set_cursor(0, 0);
            lcd_print(buf);

            delay_ms_counted(SCROLL_DELAY_MS);
            elapsed += SCROLL_DELAY_MS;

            if (elapsed >= DISPLAY_DURATION_MS) return;
        }
    }
}

/* Blink text on/off for DISPLAY_DURATION_MS */
static void show_blink(const Ad *ad) {
    uint32_t elapsed = 0;
    uint8_t  visible = 1;

    while (elapsed < DISPLAY_DURATION_MS) {
        if (visible) {
            lcd_clear();
            print_centred(ad->line1, 0);
            if (ad->line2) print_centred(ad->line2, 1);
            delay_ms_counted(BLINK_ON_MS);
            elapsed += BLINK_ON_MS;
        } else {
            lcd_clear();
            delay_ms_counted(BLINK_OFF_MS);
            elapsed += BLINK_OFF_MS;
        }
        visible = !visible;
    }
}

/* Dispatch to the right display function */
static void show_ad(const Ad *ad) {
    switch (ad->mode) {
        case MODE_SCROLL: show_scroll(ad); break;
        case MODE_BLINK:  show_blink(ad);  break;
        default:          show_static(ad); break;
    }
}

/* ------------------------------------------------------------------ */
/*  Main                                                                */
/* ------------------------------------------------------------------ */

int main(void) {
    lcd_init();

    /* Seed rand with a compile-time value (no true RNG on Uno) */
    srand(42);

    /* Show a brief startup message */
    lcd_clear();
    lcd_set_cursor(2, 0);
    lcd_print("IOT Reklambyr a");
    lcd_set_cursor(4, 1);
    lcd_print("Starting...");
    delay_ms_counted(2000);

    uint8_t last_customer = 0xFF; /* No previous customer */

    while (1) {
        uint8_t  cust_idx  = pick_next_customer(last_customer);
        const Customer *c  = &customers[cust_idx];
        uint8_t  ad_idx    = pick_ad(c, uptime_minutes());
        const Ad *ad       = &c->ads[ad_idx];

        show_ad(ad);

        last_customer = cust_idx;
    }

    return 0; /* Never reached */
}
