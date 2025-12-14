/*
This is a modified version of the systick interface that allows the main
program to register callbacks and retrieve the system time.

Angelo DiTocco
*/

#ifndef SYSTICK_H
#define SYSTICK_H

#include <stdbool.h>
#include <stdint.h>

void configure_systick();
_Bool register_systick_callback(void (*cb)());
_Bool system_tick();
uint64_t get_system_time();

#endif