#ifndef LCD_H
#define LCD_H

#include <avr/io.h>
#include <stdint.h>

// ---------------- PIN CONFIGURATION ----------------
// Using 4-bit mode (RS, EN, D4–D7)

// RS
#define LCD_RS_PORT  PORTB
#define LCD_RS_DDR   DDRB
#define LCD_RS_PIN   PB4

// EN
#define LCD_EN_PORT  PORTB
#define LCD_EN_DDR   DDRB
#define LCD_EN_PIN   PB3

// Data pins
#define LCD_D4_PORT  PORTD
#define LCD_D4_DDR   DDRD
#define LCD_D4_PIN   PD5

#define LCD_D5_PORT  PORTD
#define LCD_D5_DDR   DDRD
#define LCD_D5_PIN   PD4

#define LCD_D6_PORT  PORTD
#define LCD_D6_DDR   DDRD
#define LCD_D6_PIN   PD3

#define LCD_D7_PORT  PORTD
#define LCD_D7_DDR   DDRD
#define LCD_D7_PIN   PD2

// ---------------- FUNCTION PROTOTYPES ----------------

void lcd_init(void);
void lcd_clear(void);
void lcd_home(void);
void lcd_set_cursor(uint8_t row, uint8_t col);
void lcd_print(const char* text);
void lcd_print_second_row(const char* text);
void lcd_scroll(const char *text, int delay_ms);
void lcd_blink(const char *text, int times, int delay_ms);

#endif
