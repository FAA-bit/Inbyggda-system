#ifndef LCD_H
#define LCD_H

#include <stdint.h>

/* Pin mapping 
   RS -> PD2
   E  -> PD3
   D4 -> PD4
   D5 -> PD5
   D6 -> PD6
   D7 -> PD7
*/

void lcd_init(void);
void lcd_clear(void);
void lcd_home(void);
void lcd_set_cursor(uint8_t col, uint8_t row);
void lcd_print(const char *str);
void lcd_print_char(char c);

/* Display control */
void lcd_display_on(void);
void lcd_display_off(void);
void lcd_blink_on(void);
void lcd_blink_off(void);
void lcd_cursor_on(void);
void lcd_cursor_off(void);

#endif /* LCD_H */
