#include "Ads.h"
#include <stdlib.h>  /* rand() */

/* ------------------------------------------------------------------ */
/*  Ad pools per customer                                               */
/* ------------------------------------------------------------------ */

static const Ad harry_ads[] = {
    { "Kop bil hos",    "Harry",               MODE_SCROLL },
    { "En god bilaffar", "(for Harry!)",        MODE_STATIC },
    { "Hederlige",      "Harrys Bilar",        MODE_BLINK  },
};

static const Ad farmor_ads[] = {
    { "Kop paj hos",    "Farmor Anka",         MODE_SCROLL },
    { "Skynda innan",   "Marten atit pajer",   MODE_STATIC },
};

/* Petter: ad selection is time-based, not random.
   Index 0 = even minutes, index 1 = odd minutes. */
static const Ad petter_ads[] = {
    { "Lat Petter",     "bygga at dig",        MODE_SCROLL },
    { "Bygga svart?",   "Ring Petter",         MODE_STATIC },
};

static const Ad langben_ads[] = {
    { "Mysterier?",     "Ring Langben",        MODE_STATIC },
    { "Langben",        "fixar biffen",        MODE_STATIC },
};

static const Ad iot_ads[] = {
    { "Synas har?",     "IOT:s Reklambyra",   MODE_STATIC },
};

/* ------------------------------------------------------------------ */
/*  Customer table                                                      */
/*  weight = payment / 1000, rounded (min 1)                           */
/*  Harry 5000->5, Langben 4000->4, Farmor 3000->3,                   */
/*  Petter 1500->2, IOT 1000->1                                       */
/* ------------------------------------------------------------------ */

const Customer customers[] = {
    { "Harry",   5, harry_ads,   3 },
    { "Langben", 4, langben_ads, 2 },
    { "Farmor",  3, farmor_ads,  2 },
    { "Petter",  2, petter_ads,  2 },
    { "IOT",     1, iot_ads,     1 },
};

const uint8_t customer_count = 5;

/* ------------------------------------------------------------------ */
/*  Weighted random selection                                           */
/*                                                                      */
/*  Build a virtual pool: Harry gets 5 tickets, Langben 4, etc.       */
/*  Pick a random ticket, map back to customer index.                  */
/*  If it would repeat last_idx, resample (max 20 tries).             */
/* ------------------------------------------------------------------ */

static uint8_t total_weight(void) {
    uint8_t total = 0;
    for (uint8_t i = 0; i < customer_count; i++)
        total += customers[i].weight;
    return total;
}

uint8_t pick_next_customer(uint8_t last_idx) {
    uint8_t total = total_weight();

    for (uint8_t attempt = 0; attempt < 20; attempt++) {
        int ticket = rand() % total;
        uint8_t acc = 0;
        for (uint8_t i = 0; i < customer_count; i++) {
            acc += customers[i].weight;
            if (ticket < acc) {
                if (i != last_idx)
                    return i;
                break; /* Same as last - resample */
            }
        }
    }

    /* Fallback: return any customer that isn't last_idx */
    for (uint8_t i = 0; i < customer_count; i++) {
        if (i != last_idx) return i;
    }
    return 0;
}

/* ------------------------------------------------------------------ */
/*  Ad selection per customer                                           */
/* ------------------------------------------------------------------ */

uint8_t pick_ad(const Customer *c, uint32_t uptime_minutes) {
    /* Petter uses time-based selection */
    if (c->ads == petter_ads) {
        return (uptime_minutes % 2 == 0) ? 0 : 1;
    }
    /* Everyone else: random */
    return (uint8_t)(rand() % c->ad_count);
}
