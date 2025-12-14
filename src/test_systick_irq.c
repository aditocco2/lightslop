/*
This test program implements two systick callbacks: one that flashes the LED
at 1 Hz and one that echoes any typed characters back to the serial terminal.
The main loop prints a dash every 5 seconds that a character is not typed.

Angelo DiTocco
*/

#include "systick.h"
#include "led.h"
#include "interrupt.h"
#include "usbcdc.h"
#include <stdint.h>

void flash_led_1_Hz(void);
void echo_typed_char(void);
void print_dash_after_5_s(void);

static uint64_t time_last_char_was_typed;

void main(void){

    __disable_irq();

    configure_led();
    configure_usbcdc();
    configure_systick();
    
    register_systick_callback(flash_led_1_Hz);
    register_systick_callback(echo_typed_char);

    __enable_irq();

    while(1){
        __wait_for_interrupt();
        if(!system_tick()){
            continue;
        }

        print_dash_after_5_s();
    }
}

void flash_led_1_Hz(void){
    static uint16_t cntr = 0;
    cntr++;
    if (cntr >= 500){
        toggle_led();
        cntr = 0;
    }
}

void echo_typed_char(void){
    char c;
    if( usbcdc_getchar(&c)){
		usbcdc_putchar(c);
        time_last_char_was_typed = get_system_time();
	}
}

void print_dash_after_5_s(void){
    uint64_t elapsed_time = get_system_time() - time_last_char_was_typed;
    if (elapsed_time >= 5000){
        usbcdc_putchar('-');
        time_last_char_was_typed = get_system_time();
    }
}