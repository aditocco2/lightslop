/*
Interface for the LED matrix driver specifying which of its functions can be
used externally.

Angelo DiTocco
*/

#ifndef LED_MATRICES_H
#define LED_MATRICES_H

#include <stdint.h>

void configure_led_matrices(void);
void set_pixel(uint8_t column, uint8_t row, uint8_t matrix_num);
void clear_pixel(uint8_t column, uint8_t row, uint8_t matrix_num);
uint8_t get_pixel(uint8_t column, uint8_t row, uint8_t matrix_num);
void print_symbol(uint8_t symbol[8], uint8_t matrix_num);
void clear_rows(uint8_t top, uint8_t bottom);
void get_rows(uint8_t return_rows[], uint8_t top, uint8_t bottom);
void clear_matrix(uint8_t matrix_num);
void clear_screen(void);
void update_display(void);

#endif