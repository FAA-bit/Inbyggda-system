#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>
#include <string.h>
#include "lcd.h"

#define INTERVAL_MS 20000
#define DELAY_STEP 100

// Säker delay som klarar långa tider
void safe_delay_ms(unsigned long ms) {
    while (ms > 0) {
        _delay_ms(DELAY_STEP);
        ms -= DELAY_STEP;
    }
}

// Kunddata

typedef struct {
    const char *name;
    int weight;
    const char *messages[3];
    int messageCount;
    int type[3];  // 0=text, 1=scroll, 2=blink
} Customer;

// Kundlista
Customer customers[] = {
   { "Hederlige Harry", 5000,
      { "Köp bil hos Harry", "En god bilaffär (for Harry!)", "Hederlige Harrys Bilar" },
      3, {1,0,2} },

    { "Farmor Anka", 3000,
      { "Köp paj hos Farmor Anka", "Skynda innan Mårten ätit alla pajer" },
      2, {1,0} },

    { "Svarte Petter", 1500,
      { "Låt Petter bygga åt dig", "Bygga svart? Ring Petter" },
      2, {1,0} },

    { "Langben", 4000,
      { "Mysterier? Ring Långben", "Långben fixar biffen" },
      2, {0,0} },

    { "IOT Reklam", 1000,
      { "Synas här? IOT:s Reklambyrå" },
      1, {0} }
};
#define CUSTOMER_COUNT (sizeof(customers)/sizeof(customers[0]))


//   VIKTAD SLUMP

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

//   VISA MEDDELANDE

void showMessage(Customer *c) {
    lcd_clear();
    lcd_set_cursor(0, 0);
    lcd_print(c->name);

    int index = rand() % c->messageCount;
    const char *msg = c->messages[index];
    int type = c->type[index];

    // Specialfall: Svarte Petter
    if (strcmp(c->name, "Svarte Petter") == 0) {
        static int fake_minute = 0;
        fake_minute++;

        if (fake_minute % 2 == 0) {
            msg = c->messages[0];
            type = 1; // scroll
        } else {
            msg = c->messages[1];
            type = 0; // text
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

    safe_delay_ms(INTERVAL_MS);
}


//   MAIN

int main(void) {
    srand(TCNT0);  // enkel seed

    lcd_init();
    lcd_print("The billboard");
    lcd_set_cursor(1, 0);
    lcd_print("Starting...");
    _delay_ms(2000);

    while (1) {
        int id = pickCustomer();
        showMessage(&customers[id]);
    }

    return 0;
}
