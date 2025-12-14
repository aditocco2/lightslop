/*
Interface for the Five Buttons driver specifying which functions can be used
externally.

Angelo DiTocco
*/

#ifndef FIVE_BUTTONS_H
#define FIVE_BUTTONS_H

#include <rp2350/resets.h>
#include <rp2350/sio.h>
#include <rp2350/io_bank0.h>
#include <rp2350/pads_bank0.h>
#include <stdbool.h>

void configure_buttons(void);
_Bool btnU_is_down_debounced(void);
_Bool btnD_is_down_debounced(void);
_Bool btnL_is_down_debounced(void);
_Bool btnR_is_down_debounced(void);
_Bool btnC_is_down_debounced(void);
_Bool btnU_is_pressed(void);
_Bool btnD_is_pressed(void);
_Bool btnL_is_pressed(void);
_Bool btnR_is_pressed(void);
_Bool btnC_is_pressed(void);

#endif