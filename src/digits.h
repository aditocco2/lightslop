/*
Interface for digits.c specifying which of its functions can be used
externally.

Angelo DiTocco
*/

#ifndef DIGITS_H
#define DIGITS_H

#include <stdint.h>
#include <led_matrices.h>

void print_number(uint8_t number, uint8_t matrix_num);

#endif