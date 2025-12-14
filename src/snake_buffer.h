/*
Interface for snake_buffer.c specifying which of its functions can be used
externally.

Angelo DiTocco
*/

#ifndef SNAKE_BUFFER_H
#define SNAKE_BUFFER_H

#include <stdint.h>
#include <stdbool.h>
#include <snake_data.h>

void reset_snake_buffer(void);
void write_to_snake_buffer(coordinate coord);
coordinate read_from_snake_buffer(void);
uint8_t get_full_spaces(void);
uint8_t get_empty_spaces(void);
_Bool snake_buffer_is_full(void);
_Bool collision(void);

#endif