#ifndef LED_MATRIX_H
#define LED_MATRIX_H

#include <stdint.h>

void configure_led_matrix(void);
void set_pixel(uint8_t x, uint8_t y);
void clear_pixel(uint8_t x, uint8_t y);
void update_display(void);

#endif