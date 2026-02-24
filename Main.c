#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdlib.h>
#include <string.h>
#include <util/delay.h>
#include "lcd.h"

#define INTERVAL_MS 20000

volatile unsigned long millis_counter = 0;

ISR(TIMER0_COMPA_vect) {
    millis_counter++;
}

void timer0_init(void) {
    TCCR0A = (1 << WGM01);
    OCR0A = 249;
    TCCR0B = (1 << CS01) | (1 << CS00);
    TIMSK0 = (1 << OCIE0A);
    sei();
}

unsigned long millis(void) {
    return millis_counter;
}

typedef struct {
    const char *name;
    int weight;
    const char *messages[3];
    int messageCount;
    int type[3]; // 0 = text, 1 = scrolling, 2 = blinking
} Customer;

Customer customers[] = {

    { "Harry", 5000,
      { "Kop bil hos Harry",
        "En god bilaffar",
        "Harrys Bilar" },
      3, {1,0,2} },

    { "Farmor Anka", 3000,
      { "Kop paj hos Farmor Anka",
        "Skynda innan Morten atit alla pajer" },
      2, {1,0} },

    { "Petter", 1500,
      { "Lat Petter bygga at dig",
        "Bygga svart? Ring Petter" },
      2, {1,0} },

    { "Langben", 4000,
      { "Mysterier? Ring Langben",
        "Langben fixar biffen" },
      2, {0,0} },

    { "IOT Reklam", 1000,
      { "Synas har? IOT:s Reklambyra" },
      1, {0} }
};

#define CUSTOMER_COUNT (sizeof(customers)/sizeof(customers[0]))

int lastCustomer = -1;

int pickCustomer(void) {

    int total = 0;
    for (int i = 0; i < CUSTOMER_COUNT; i++)
        total += customers[i].weight;

    int chosen;

    do {
        int r = rand() % total;
        int sum = 0;

        for (int i = 0; i < CUSTOMER_COUNT; i++) {
            sum += customers[i].weight;
            if (r < sum) {
                chosen = i;
                break;
            }
        }

    } while (chosen == lastCustomer);

    lastCustomer = chosen;
    return chosen;
}

void showMessage(Customer *c) {

    lcd_clear();
    lcd_set_cursor(0, 0);
    lcd_print(c->name);

    int index = rand() % c->messageCount;
    const char *msg = c->messages[index];
    int type = c->type[index];

    // Special rule for Petter (even/odd minute simulation)
    if (strcmp(c->name, "Petter") == 0) {

        unsigned long minute = millis() / 60000;

        if (minute % 2 == 0) {
            msg = c->messages[0];
            type = 1;
        } else {
            msg = c->messages[1];
            type = 0;
        }
    }

    lcd_set_cursor(1, 0);

    switch (type) {
        case 0:
            lcd_print(msg);
            break;

        case 1:
            lcd_scroll(msg, 300);
            break;

        case 2:
            lcd_blink(msg, 6, 300);
            break;
    }
}

int main(void) {

    timer0_init();
    srand(millis());

    lcd_init();

    lcd_print("The billboard");
    lcd_set_cursor(1, 0);
    lcd_print("Starting...");
    _delay_ms(2000);

    unsigned long lastChange = 0;

    while (1) {

        unsigned long now = millis();

        if (now - lastChange >= INTERVAL_MS) {

            lastChange = now;

            int id = pickCustomer();
            showMessage(&customers[id]);
        }
    }
}
