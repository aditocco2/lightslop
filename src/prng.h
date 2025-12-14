/*
Interface for the MLCG specifying which of its functions can be used
externally.

Angelo DiTocco
*/

#ifndef PRNG_H
#define PRNG_H

#include <stdint.h>

void seed_prng(uint32_t seed);
void update_prng(void);
uint32_t get_prng(void);

#endif