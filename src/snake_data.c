/*
Determines whether two coordinates, as defined in snake_data.h, are equal.

Angelo DiTocco
*/

#include "snake_data.h"
#include <stdint.h>

_Bool coordinates_equal(coordinate coord1, coordinate coord2){
    return ((coord1.x == coord2.x) && (coord1.y == coord2.y));
}