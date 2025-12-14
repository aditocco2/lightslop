/*
Defines the data types for the X and Y coordinates and the facing direction of
the snake. Also an interface for a function checking for coordinate equality.

Angelo DiTocco
*/

#ifndef SNAKE_DATA_H
#define SNAKE_DATA_H

#include <stdint.h>
#include <stdbool.h>

typedef struct{
    uint8_t x;
    uint8_t y;
} coordinate;

typedef enum {UP, DOWN, LEFT, RIGHT} direction;

_Bool coordinates_equal(coordinate coord1, coordinate coord2);

#endif