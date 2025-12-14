/*
This is a test program that scans the keypad every millisecond and prints
any characters it detects onto the USB serial terminal.

Angelo DiTocco
*/

#include <keypad.h>
#include <usbcdc.h>
#include <systick.h>
#include <interrupt.h>

static char c;

void flash_led_1_Hz(void);

void main(void){
   
    __disable_irq();
    configure_keypad();
    configure_usbcdc();
    configure_systick();
    __enable_irq();

    register_systick_callback(task_keypad_scan);

    while(1){
        __wait_for_interrupt();
        if(!system_tick()){
            continue;
        }

        if (keypad_getchar(&c)){
            usbcdc_putchar(c);
        }
    }
}