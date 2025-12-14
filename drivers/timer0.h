/*
Interface for the timer driver specifying which functions can be used
externally.

Angelo DiTocco
*/

#ifndef TIMER0_H
#define TIMER0_H

#include <stdbool.h>
#include <stdint.h>

void configure_timer0(void);
void set_alarm(uint8_t alarm_number, uint32_t time_us,
                void (*pf)(), uint8_t priority_8b);

#endif