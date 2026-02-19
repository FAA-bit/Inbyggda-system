#include "lcd.h"
#include <util/delay.h>
#include <string.h>

//   HJÄLPMAKRO: Sätt pin HIGH/LOW

#define SET_PIN(port, pin)   (port |=  (1 << pin))
#define CLR_PIN(port, pin)   (port &= ~(1 << pin))

//   SKICKA NIBBLE (4 bitar)

static void lcd_send_nibble(uint8_t nibble) {
    // D4
    if (nibble & 0x01) SET_PIN(LCD_D4_PORT, LCD_D4_PIN);
    else               CLR_PIN(LCD_D4_PORT, LCD_D4_PIN);

    // D5
    if (nibble & 0x02) SET_PIN(LCD_D5_PORT, LCD_D5_PIN);
    else               CLR_PIN(LCD_D5_PORT, LCD_D5_PIN);

    // D6
    if (nibble & 0x04) SET_PIN(LCD_D6_PORT, LCD_D6_PIN);
    else               CLR_PIN(LCD_D6_PORT, LCD_D6_PIN);

    // D7
    if (nibble & 0x08) SET_PIN(LCD_D7_PORT, LCD_D7_PIN);
    else               CLR_PIN(LCD_D7_PORT, LCD_D7_PIN);

    // EN pulse
    SET_PIN(LCD_EN_PORT, LCD_EN_PIN);
    _delay_us(1);
    CLR_PIN(LCD_EN_PORT, LCD_EN_PIN);
    _delay_us(100);
}

//   SKICKA KOMMANDO ELLER DATA

static void lcd_send_byte(uint8_t byte, uint8_t isData) {
    if (isData)
        SET_PIN(LCD_RS_PORT, LCD_RS_PIN);
    else
        CLR_PIN(LCD_RS_PORT, LCD_RS_PIN);

    lcd_send_nibble(byte >> 4);   // övre nibble
    lcd_send_nibble(byte & 0x0F); // nedre nibble
}

//   OFFENTLIGA FUNKTIONER

void lcd_init(void) {
    // Sätt pinnar som utgångar
    DDRB |= (1 << LCD_RS_PIN) | (1 << LCD_EN_PIN);
    DDRD |= (1 << LCD_D4_PIN) | (1 << LCD_D5_PIN) |
            (1 << LCD_D6_PIN) | (1 << LCD_D7_PIN);

    _delay_ms(50);

    // Init-sekvens enligt HD44780-datasheet
    lcd_send_nibble(0x03);
    _delay_ms(5);

    lcd_send_nibble(0x03);
    _delay_us(150);

    lcd_send_nibble(0x03);
    lcd_send_nibble(0x02); // 4-bitarsläge

    // Funktion set: 4-bit, 2 rader, 5x8 font
    lcd_send_byte(0x28, 0);

    // Display ON, cursor OFF
    lcd_send_byte(0x0C, 0);

    // Clear display
    lcd_send_byte(0x01, 0);
    _delay_ms(2);

    // Entry mode: auto-increment
    lcd_send_byte(0x06, 0);
}

void lcd_clear(void) {
    lcd_send_byte(0x01, 0);
    _delay_ms(2);
}

void lcd_home(void) {
    lcd_send_byte(0x02, 0);
    _delay_ms(2);
}

void lcd_set_cursor(uint8_t row, uint8_t col) {
    uint8_t addr = (row == 0) ? 0x00 + col : 0x40 + col;
    lcd_send_byte(0x80 | addr, 0);
}

void lcd_print(const char *text) {
    while (*text) {
        lcd_send_byte(*text++, 1);
    }
}

void lcd_print_second_row(const char *text) {
    lcd_set_cursor(1, 0);
    lcd_print(text);
}

//   SCROLL

void lcd_scroll(const char *text, int delay_ms) {
    int len = strlen(text);

    if (len <= 16) {
        lcd_print(text);
        return;
    }

    for (int i = 0; i <= len - 16; i++) {
        lcd_set_cursor(1, 0);
        lcd_print("                ");  // 16 spaces
        lcd_set_cursor(1, 0);
        lcd_print(text + i);
        _delay_ms(delay_ms);
    }
}

//   BLINK

void lcd_blink(const char *text, int times, int delay_ms) {
    for (int i = 0; i < times; i++) {
        lcd_clear();
        _delay_ms(delay_ms);

        lcd_set_cursor(1, 0);
        lcd_print(text);
        _delay_ms(delay_ms);
    }
}

