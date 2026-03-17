#ifndef ADS_H
#define ADS_H

#include <stdint.h>

/* Display modes */
typedef enum {
    MODE_STATIC,   /* Show text statically for full duration */
    MODE_SCROLL,   /* Scroll text across display */
    MODE_BLINK     /* Blink text on/off */
} DisplayMode;

/* A single advertisement message */
typedef struct {
    const char   *line1;      /* First LCD row  (max 16 chars) */
    const char   *line2;      /* Second LCD row (max 16 chars, can be NULL) */
    DisplayMode   mode;
} Ad;

/* A customer with a pool of ads and a weight */
typedef struct {
    const char  *name;
    uint8_t      weight;      /* Proportional to payment (1000 kr = 1 unit) */
    const Ad    *ads;
    uint8_t      ad_count;
} Customer;

/* --- Customer data --- */
extern const Customer customers[];
extern const uint8_t  customer_count;

/* Pick next customer index - never same as last_idx */
uint8_t pick_next_customer(uint8_t last_idx);

/* Pick a random ad index for a given customer */
uint8_t pick_ad(const Customer *c, uint32_t uptime_minutes);

#endif /* ADS_H */
