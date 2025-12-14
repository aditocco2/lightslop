/*
Interface specifying what functions on the button driver can be used
externally.

Angelo DiTocco
*/

#ifndef BUTTON_H
#define BUTTON_H

#include <rp2350/resets.h>
#include <rp2350/sio.h>
#include <rp2350/io_bank0.h>
#include <rp2350/pads_bank0.h>
#include <stdbool.h>

typedef enum {NO_PRESS, SHORT_PRESS, PRESS, LONG_PRESS} button_press_t;
void configure_button(void);
button_press_t get_button_press(void);

#endif