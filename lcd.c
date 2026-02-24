#include "lcd.h"
#include <util/delay.h>

#define SET_PIN(port, pin)   (port |=  (1 << pin))
#define CLR_PIN(port, pin)   (port &= ~(1 << pin))

static uint8_t convert_char(char c);

static void lcd_send_nibble(uint8_t nibble) {

    if (nibble & 0x01) SET_PIN(LCD_D4_PORT, LCD_D4_PIN);
    else CLR_PIN(LCD_D4_PORT, LCD_D4_PIN);

    if (nibble & 0x02) SET_PIN(LCD_D5_PORT, LCD_D5_PIN);
    else CLR_PIN(LCD_D5_PORT, LCD_D5_PIN);

    if (nibble & 0x04) SET_PIN(LCD_D6_PORT, LCD_D6_PIN);
    else CLR_PIN(LCD_D6_PORT, LCD_D6_PIN);

    if (nibble & 0x08) SET_PIN(LCD_D7_PORT, LCD_D7_PIN);
    else CLR_PIN(LCD_D7_PORT, LCD_D7_PIN);

    SET_PIN(LCD_EN_PORT, LCD_EN_PIN);
    _delay_us(1);
    CLR_PIN(LCD_EN_PORT, LCD_EN_PIN);
    _delay_us(100);
}

static void lcd_send_byte(uint8_t byte, uint8_t isData) {

    if (isData)
        SET_PIN(LCD_RS_PORT, LCD_RS_PIN);
    else
        CLR_PIN(LCD_RS_PORT, LCD_RS_PIN);

    lcd_send_nibble(byte >> 4);
    lcd_send_nibble(byte & 0x0F);
}

void lcd_init(void) {

    LCD_RS_DDR |= (1 << LCD_RS_PIN);
    LCD_EN_DDR |= (1 << LCD_EN_PIN);

    LCD_D4_DDR |= (1 << LCD_D4_PIN);
    LCD_D5_DDR |= (1 << LCD_D5_PIN);
    LCD_D6_DDR |= (1 << LCD_D6_PIN);
    LCD_D7_DDR |= (1 << LCD_D7_PIN);

    _delay_ms(50);

    lcd_send_nibble(0x03);
    _delay_ms(5);
    lcd_send_nibble(0x03);
    _delay_us(150);
    lcd_send_nibble(0x03);
    lcd_send_nibble(0x02);

    lcd_send_byte(0x28, 0);
    lcd_send_byte(0x0C, 0);
    lcd_send_byte(0x01, 0);
    _delay_ms(2);
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
    uint8_t addr = (row == 0) ? col : (0x40 + col);
    lcd_send_byte(0x80 | addr, 0);
}

void lcd_print(const char *text) {

    uint8_t count = 0;

    while (*text && count < 16) {
        lcd_send_byte(convert_char(*text++), 1);
        count++;
    }
}

void lcd_scroll(const char *text, int delay_ms) {

    char buffer[17] = {0};
    int len = 0;

    while (text[len]) len++;

    for (int pos = 0; pos < len + 16; pos++) {

        for (int i = 0; i < 16; i++) {
            int idx = pos - 15 + i;
            buffer[i] = (idx >= 0 && idx < len) ? text[idx] : ' ';
        }

        buffer[16] = '\0';

        lcd_set_cursor(1, 0);
        lcd_print(buffer);
        _delay_ms(delay_ms);
    }
}

void lcd_blink(const char *text, int times, int delay_ms) {

    for (int i = 0; i < times; i++) {

        lcd_set_cursor(1, 0);
        lcd_print(text);
        _delay_ms(delay_ms);

        lcd_set_cursor(1, 0);
        for (int j = 0; j < 16; j++)
            lcd_send_byte(' ', 1);

        _delay_ms(delay_ms);
    }
}

static uint8_t convert_char(char c) {

    if (c >= ' ' && c <= '~')
        return c;

    return '?';
}
