/*
Driver to pulse and debounce five buttons in an up/down/left/right/center
configuration.

Angelo DiTocco
*/

#include "five_buttons.h"
#include <rp2350/resets.h>
#include <rp2350/sio.h>
#include <rp2350/io_bank0.h>
#include <rp2350/pads_bank0.h>
#include <stdint.h>
#include <stdbool.h>

#define BTNU 16
#define BTND 19
#define BTNL 17
#define BTNR 18
#define BTNC 20

#define DEBOUNCE_TIME_MS 5

#define CONCAT2(X,Y) X ## Y
#define CONCAT3(X,Y,Z) X ## Y ## Z
#define IO_BANK0_GPIO_CTRL(X) CONCAT3(IO_BANK0_GPIO,X,_CTRL)
#define PADS_BANK0_GPIO(X) CONCAT2(PADS_BANK0_GPIO,X)
#define GPIO_RESETS (RESETS_RESET_IO_BANK0_MASK | RESETS_RESET_PADS_BANK0_MASK)

void configure_buttons(void);
_Bool btnU_is_down_debounced(void);
_Bool btnD_is_down_debounced(void);
_Bool btnL_is_down_debounced(void);
_Bool btnR_is_down_debounced(void);
_Bool btnC_is_down_debounced(void);
static _Bool button_is_up(uint8_t pin);
static _Bool button_is_down(uint8_t pin);

void configure_buttons(void){
    RESETS_RESET_CLR = GPIO_RESETS;
    while((RESETS_RESET_DONE & GPIO_RESETS ) != GPIO_RESETS )
        continue;

    uint32_t pads_bank0_config_value =
        PADS_BANK0_GPIO0_OD(1)
        | PADS_BANK0_GPIO0_IE(1)
        | PADS_BANK0_GPIO0_DRIVE(0)
        | PADS_BANK0_GPIO0_PUE(1)
        | PADS_BANK0_GPIO0_PDE(0)
        | PADS_BANK0_GPIO0_SCHMITT(0)
        | PADS_BANK0_GPIO0_SLEWFAST(0);

    uint32_t io_bank0_config_value =
        IO_BANK0_GPIO0_CTRL_IRQOVER(0)
        | IO_BANK0_GPIO0_CTRL_INOVER(0)
        | IO_BANK0_GPIO0_CTRL_OEOVER(0)
        | IO_BANK0_GPIO0_CTRL_OUTOVER(0)
        | IO_BANK0_GPIO0_CTRL_FUNCSEL(5);

    PADS_BANK0_GPIO(BTNU) = pads_bank0_config_value;
    IO_BANK0_GPIO_CTRL(BTNU) = io_bank0_config_value;
    PADS_BANK0_GPIO(BTND) = pads_bank0_config_value;
    IO_BANK0_GPIO_CTRL(BTND) = io_bank0_config_value;
    PADS_BANK0_GPIO(BTNL) = pads_bank0_config_value;
    IO_BANK0_GPIO_CTRL(BTNL) = io_bank0_config_value;
    PADS_BANK0_GPIO(BTNR) = pads_bank0_config_value;
    IO_BANK0_GPIO_CTRL(BTNR) = io_bank0_config_value;
    PADS_BANK0_GPIO(BTNC) = pads_bank0_config_value;
    IO_BANK0_GPIO_CTRL(BTNC) = io_bank0_config_value;
}

static _Bool button_is_up(uint8_t pin){
    return (SIO_GPIO_IN & (1 << pin));
}

static _Bool button_is_down(uint8_t pin){
    return !button_is_up(pin);
}

_Bool btnU_is_down_debounced(void){
    static uint8_t cntr = 0;
    static _Bool debounced_state = false;

    if (cntr < DEBOUNCE_TIME_MS && button_is_down(BTNU)){
        cntr++;
    }
    else if (cntr > 0 && button_is_up(BTNU)){
        cntr--;
    }

    if (cntr == 0){
        debounced_state = false;
    }
    else if (cntr >= DEBOUNCE_TIME_MS){
        debounced_state = true;
    }
    
    return debounced_state;
}

_Bool btnD_is_down_debounced(void){
    static uint8_t cntr = 0;
    static _Bool debounced_state = false;

    if (cntr < DEBOUNCE_TIME_MS && button_is_down(BTND)){
        cntr++;
    }
    else if (cntr > 0 && button_is_up(BTND)){
        cntr--;
    }

    if (cntr == 0){
        debounced_state = false;
    }
    else if (cntr >= DEBOUNCE_TIME_MS){
        debounced_state = true;
    }
    
    return debounced_state;
}

_Bool btnL_is_down_debounced(void){
    static uint8_t cntr = 0;
    static _Bool debounced_state = false;

    if (cntr < DEBOUNCE_TIME_MS && button_is_down(BTNL)){
        cntr++;
    }
    else if (cntr > 0 && button_is_up(BTNL)){
        cntr--;
    }

    if (cntr == 0){
        debounced_state = false;
    }
    else if (cntr >= DEBOUNCE_TIME_MS){
        debounced_state = true;
    }
    
    return debounced_state;
}

_Bool btnR_is_down_debounced(void){
    static uint8_t cntr = 0;
    static _Bool debounced_state = false;

    if (cntr < DEBOUNCE_TIME_MS && button_is_down(BTNR)){
        cntr++;
    }
    else if (cntr > 0 && button_is_up(BTNR)){
        cntr--;
    }

    if (cntr == 0){
        debounced_state = false;
    }
    else if (cntr >= DEBOUNCE_TIME_MS){
        debounced_state = true;
    }
    
    return debounced_state;
}

_Bool btnC_is_down_debounced(void){
    static uint8_t cntr = 0;
    static _Bool debounced_state = false;

    if (cntr < DEBOUNCE_TIME_MS && button_is_down(BTNC)){
        cntr++;
    }
    else if (cntr > 0 && button_is_up(BTNC)){
        cntr--;
    }

    if (cntr == 0){
        debounced_state = false;
    }
    else if (cntr >= DEBOUNCE_TIME_MS){
        debounced_state = true;
    }
    
    return debounced_state;
}

_Bool btnU_is_pressed(void){
    static _Bool state = false, prev_state = false;

    prev_state = state;
    state = btnU_is_down_debounced();

    _Bool pressed = false;
    if(!prev_state && state){
        pressed = true;
    }

    return pressed;
}

_Bool btnD_is_pressed(void){
    static _Bool state = false, prev_state = false;

    prev_state = state;
    state = btnD_is_down_debounced();

    _Bool pressed = false;
    if(!prev_state && state){
        pressed = true;
    }

    return pressed;
}

_Bool btnL_is_pressed(void){
    static _Bool state = false, prev_state = false;

    prev_state = state;
    state = btnL_is_down_debounced();

    _Bool pressed = false;
    if(!prev_state && state){
        pressed = true;
    }

    return pressed;
}

_Bool btnR_is_pressed(void){
    static _Bool state = false, prev_state = false;

    prev_state = state;
    state = btnR_is_down_debounced();

    _Bool pressed = false;
    if(!prev_state && state){
        pressed = true;
    }

    return pressed;
}

_Bool btnC_is_pressed(void){
    static _Bool state = false, prev_state = false;

    prev_state = state;
    state = btnC_is_down_debounced();

    _Bool pressed = false;
    if(!prev_state && state){
        pressed = true;
    }

    return pressed;
}