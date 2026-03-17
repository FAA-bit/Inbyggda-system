#include "lcd.h"
#include <avr/io.h>
#include <util/delay.h>

/* --- Pin definitions --- */
#define LCD_PORT  PORTD
#define LCD_DDR   DDRD

#define LCD_RS    PD2
#define LCD_E     PD3
#define LCD_D4    PD4
#define LCD_D5    PD5
#define LCD_D6    PD6
#define LCD_D7    PD7

/* --- Internal state --- */
static uint8_t _display_ctrl = 0;

/* --- Low-level helpers --- */

static void pulse_enable(void) {
    LCD_PORT |=  (1 << LCD_E);
    _delay_us(1);
    LCD_PORT &= ~(1 << LCD_E);
    _delay_us(50);
}

static void write_nibble(uint8_t nibble) {
    /* Clear data bits */
    LCD_PORT &= ~((1<<LCD_D4)|(1<<LCD_D5)|(1<<LCD_D6)|(1<<LCD_D7));
    if (nibble & 0x01) LCD_PORT |= (1 << LCD_D4);
    if (nibble & 0x02) LCD_PORT |= (1 << LCD_D5);
    if (nibble & 0x04) LCD_PORT |= (1 << LCD_D6);
    if (nibble & 0x08) LCD_PORT |= (1 << LCD_D7);
    pulse_enable();
}

static void send_byte(uint8_t byte, uint8_t is_data) {
    if (is_data)
        LCD_PORT |=  (1 << LCD_RS);
    else
        LCD_PORT &= ~(1 << LCD_RS);

    write_nibble(byte >> 4);   /* High nibble first */
    write_nibble(byte & 0x0F); /* Low nibble */
    _delay_us(50);
}

static void send_cmd(uint8_t cmd) {
    send_byte(cmd, 0);
    if (cmd <= 0x03)
        _delay_ms(2); /* Clear/home need extra time */
}

/* --- Public API --- */

void lcd_init(void) {
    /* Set all LCD pins as output */
    LCD_DDR |= (1<<LCD_RS)|(1<<LCD_E)|(1<<LCD_D4)|(1<<LCD_D5)|(1<<LCD_D6)|(1<<LCD_D7);
    LCD_PORT &= ~((1<<LCD_RS)|(1<<LCD_E)|(1<<LCD_D4)|(1<<LCD_D5)|(1<<LCD_D6)|(1<<LCD_D7));

    _delay_ms(50); /* Wait for power-up */

    /* Initialise in 4-bit mode per HD44780 datasheet */
    write_nibble(0x03); _delay_ms(5);
    write_nibble(0x03); _delay_us(150);
    write_nibble(0x03); _delay_us(150);
    write_nibble(0x02); /* Switch to 4-bit */

    send_cmd(0x28); /* 4-bit, 2 lines, 5x8 font */

    _display_ctrl = 0x0C; /* Display on, cursor off, blink off */
    send_cmd(_display_ctrl);

    send_cmd(0x01); /* Clear display */
    _delay_ms(2);

    send_cmd(0x06); /* Entry mode: increment, no shift */
}

void lcd_clear(void) {
    send_cmd(0x01);
    _delay_ms(2);
}

void lcd_home(void) {
    send_cmd(0x02);
    _delay_ms(2);
}

void lcd_set_cursor(uint8_t col, uint8_t row) {
    uint8_t row_offsets[] = {0x00, 0x40};
    if (row > 1) row = 1;
    if (col > 15) col = 15;
    send_cmd(0x80 | (col + row_offsets[row]));
}

void lcd_print_char(char c) {
    send_byte((uint8_t)c, 1);
}

void lcd_print(const char *str) {
    while (*str) {
        lcd_print_char(*str++);
    }
}

void lcd_display_on(void)  { _display_ctrl |=  0x04; send_cmd(_display_ctrl); }
void lcd_display_off(void) { _display_ctrl &= ~0x04; send_cmd(_display_ctrl); }
void lcd_blink_on(void)    { _display_ctrl |=  0x01; send_cmd(_display_ctrl); }
void lcd_blink_off(void)   { _display_ctrl &= ~0x01; send_cmd(_display_ctrl); }
void lcd_cursor_on(void)   { _display_ctrl |=  0x02; send_cmd(_display_ctrl); }
void lcd_cursor_off(void)  { _display_ctrl &= ~0x02; send_cmd(_display_ctrl); }
