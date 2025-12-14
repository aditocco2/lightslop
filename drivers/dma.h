/*
Interface for the DMA driver specifying which of its functions can be
used externally.

Angelo DiTocco
*/

#ifndef DMA_H
#define DMA_H

#include <stdint.h>

void configure_dma(void);

void memcpy8(uint8_t *dest, const uint8_t *src, uint32_t n);
void memcpy16(uint16_t *dest, const uint16_t *src, uint32_t n);
void memcpy32(uint32_t *dest, const uint32_t *src, uint32_t n);

void clear8(uint8_t *dest, uint32_t n);
void clear16(uint16_t *dest, uint32_t n);
void clear32(uint32_t *dest, uint32_t n);

void wait_until_dma_is_free(void);

#endif