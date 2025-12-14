/*
Driver that debounces the button and differentiates between a short press,
medium press, and long press.

Angelo DiTocco
*/

#include "button.h"
#include <rp2350/resets.h>
#include <rp2350/sio.h>
#include <rp2350/io_bank0.h>
#include <rp2350/pads_bank0.h>
#include <stdbool.h>

#define BTN_LOC 21

#define DEBOUNCE_TIME_MS 5

#define PRESS_DURATION_MS 500
#define LONG_PRESS_DURATION_MS 1000

#define CONCAT2(X,Y) X ## Y
#define CONCAT3(X,Y,Z) X ## Y ## Z
#define IO_BANK0_GPIO_CTRL(X) CONCAT3(IO_BANK0_GPIO,X,_CTRL)
#define PADS_BANK0_GPIO(X) CONCAT2(PADS_BANK0_GPIO,X)
#define GPIO_RESETS (RESETS_RESET_IO_BANK0_MASK | RESETS_RESET_PADS_BANK0_MASK)

_Bool button_is_up(void);
_Bool button_is_down(void);
_Bool button_is_down_debounced(void);

void configure_button(void){
    RESETS_RESET_CLR = GPIO_RESETS;
    while((RESETS_RESET_DONE & GPIO_RESETS ) != GPIO_RESETS )
        continue;

    PADS_BANK0_GPIO(BTN_LOC) =
        PADS_BANK0_GPIO0_OD(1)
        | PADS_BANK0_GPIO0_IE(1)
        | PADS_BANK0_GPIO0_DRIVE(0)
        | PADS_BANK0_GPIO0_PUE(1)
        | PADS_BANK0_GPIO0_PDE(0)
        | PADS_BANK0_GPIO0_SCHMITT(0)
        | PADS_BANK0_GPIO0_SLEWFAST(0);

    IO_BANK0_GPIO_CTRL(BTN_LOC) =
        IO_BANK0_GPIO0_CTRL_IRQOVER(0)
        | IO_BANK0_GPIO0_CTRL_INOVER(0)
        | IO_BANK0_GPIO0_CTRL_OEOVER(0)
        | IO_BANK0_GPIO0_CTRL_OUTOVER(0)
        | IO_BANK0_GPIO0_CTRL_FUNCSEL(5);
}

button_press_t get_button_press(void){
    static enum {S0_NOT_PRESSED, S1_SHORT_PRESS, S2_PRESS, S3_LONG_PRESS} state;
    static uint16_t cntr = 0;

    static button_press_t retval;
    retval = NO_PRESS;

    _Bool button_is_pressed = button_is_down_debounced();

    switch(state){
        case S0_NOT_PRESSED:
            if (button_is_pressed){
                state = S1_SHORT_PRESS;
                cntr = 0;
            }
            break;
        case S1_SHORT_PRESS:
            cntr++;
            if (!button_is_pressed){
                state = S0_NOT_PRESSED;
                retval = SHORT_PRESS;
            }
            else if (button_is_pressed && cntr >= PRESS_DURATION_MS){
                state = S2_PRESS;
            }
            break;
        case S2_PRESS:
            cntr++;
            if (!button_is_pressed){
                state = S0_NOT_PRESSED;
                retval = PRESS;
            }
            if (button_is_pressed && cntr >= LONG_PRESS_DURATION_MS){
                state = S3_LONG_PRESS;
                retval = LONG_PRESS;
            }
            break;
        case S3_LONG_PRESS:
            if (!button_is_pressed){
                state = S0_NOT_PRESSED;
            }
            break;
    }

    return retval;
}

_Bool button_is_up(void){
    return (SIO_GPIO_IN && (1 << BTN_LOC));
}

_Bool button_is_down(void){
    return !button_is_up();
}

_Bool button_is_down_debounced(void){
    static uint8_t cntr = 0;
    static _Bool debounced_state = false;

    if (cntr < DEBOUNCE_TIME_MS && button_is_down()){
        cntr++;
    }
    else if (cntr > 0 && button_is_up()){
        cntr--;
    }

    if (cntr == 0){
        debounced_state = false;
    }
    else if (cntr == DEBOUNCE_TIME_MS){
        debounced_state = true;
    }
    
    return debounced_state;
}