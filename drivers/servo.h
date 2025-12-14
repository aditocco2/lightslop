/*
This is an interface for the servo which allows the configuration
and rotation functions to be used externally by the main program.

Angelo DiTocco
*/

#ifndef SERVO_H
#define SERVO_H

#include <stdint.h>

void configure_servo(void);
void rotate_servo(int16_t rotate);

#endif