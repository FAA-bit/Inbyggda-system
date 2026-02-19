// lcd.h – Driver för HD44780 LCD i 4-bitarsläge
// För Arduino Uno (ATmega328P)
// Av: Faid, IOT25

#ifndef LCD_H
#define LCD_H

#include <stdint.h>
#include <avr/io.h>

// RS, EN och D4–D7 används
#define LCD_RS_PORT  PORTB
#define LCD_RS_PIN   PB4    // Pin 12

#define LCD_EN_PORT  PORTB
#define LCD_EN_PIN   PB3    // Pin 11

#define LCD_D4_PORT  PORTD
#define LCD_D4_PIN   PD5    // Pin 5

#define LCD_D5_PORT  PORTD
#define LCD_D5_PIN   PD4    // Pin 4

#define LCD_D6_PORT  PORTD
#define LCD_D6_PIN   PD3    // Pin 3

#define LCD_D7_PORT  PORTD
#define LCD_D7_PIN   PD2    // Pin 2

// Funktioner
void lcd_init(void);
void lcd_clear(void);
void lcd_home(void);
void lcd_set_cursor(uint8_t row, uint8_t col);
void lcd_print(const char* text);
void lcd_print_second_row(const char* text);
void lcd_scroll(const char *text, int delay_ms);
void lcd_blink(const char *text, int times, int delay_ms);

#endif
