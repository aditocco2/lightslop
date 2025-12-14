/*
This is a PRNG (Park-Miller multiplicative linear congruential generator)
used to pick random locations at which to generate food.

Angelo DiTocco
*/

#include "prng.h"
#include <stdint.h>

#define MULTIPLIER 48271
#define MODULUS 0x7FFFFFFF

void seed_prng(uint32_t seed);
void update_prng(void);
uint32_t get_prng(void);

static uint32_t prng;

void seed_prng(uint32_t seed){
    prng = seed;
}

void update_prng(void){
    prng = (uint64_t)prng * MULTIPLIER % MODULUS;
}

uint32_t get_prng(void){
    return prng;
}