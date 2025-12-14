/*
Uses the newly made button driver to control the LED. Switches between fast
blinking, slow blinking, and being off with a short, medium and long press
respectively.

Angelo DiTocco
*/

#include "button.h"
#include <led.h>
#include <systick.h>

#define SLOW_BLINK_PERIOD_MS 1000
#define FAST_BLINK_PERIOD_MS 200

void blink_led_fsm(button_press_t press_type);

void main(void){
    configure_button();
    configure_led();
    configure_systick();

    while(1){
        if (!system_tick()){
            continue;
        }

        button_press_t press_type = get_button_press();

        blink_led_fsm(press_type);
    }
}

void blink_led_fsm(button_press_t press_type){
    static enum {S0_OFF, S1_SLOW_ON, S2_SLOW_OFF, S3_FAST_ON, S4_FAST_OFF} state;
    static uint16_t cntr = 0;

    switch (state){
        case S0_OFF:
            turn_off_led();
            if (press_type == SHORT_PRESS){
                state = S1_SLOW_ON;
                turn_on_led();
                cntr = 0;
            }
            else if (press_type == PRESS){
                state = S3_FAST_ON;
                turn_on_led();
                cntr = 0;
            }
            break;
        case S1_SLOW_ON:
            cntr++;
            if (cntr > SLOW_BLINK_PERIOD_MS / 2){
                state = S2_SLOW_OFF;
                turn_off_led();
                cntr = 0;
            }
            else if (press_type == PRESS){
                state = S3_FAST_ON;
            } 
            else if (press_type == LONG_PRESS){
                state = S0_OFF;
            }
            break;
        case S2_SLOW_OFF:
            cntr++;
            if (cntr > SLOW_BLINK_PERIOD_MS / 2){
                state = S1_SLOW_ON;
                turn_on_led();
                cntr = 0;
            }
            else if (press_type == PRESS){
                state = S4_FAST_OFF;
            }
            else if (press_type == LONG_PRESS){
                state = S0_OFF;
            }
            break;
        case S3_FAST_ON:
            cntr++;
            if (cntr > FAST_BLINK_PERIOD_MS / 2){
                state = S4_FAST_OFF;
                turn_off_led();
                cntr = 0;
            }
            else if (press_type == SHORT_PRESS)
            {
                state = S1_SLOW_ON;
            }
            else if (press_type == LONG_PRESS){
                state = S0_OFF;
            }
            break;
        case S4_FAST_OFF:
            cntr++;
            if (cntr > FAST_BLINK_PERIOD_MS / 2){
                state = S3_FAST_ON;
                turn_on_led();
                cntr = 0;
            }
            else if (press_type == SHORT_PRESS)
            {
                state = S2_SLOW_OFF;
            }
            else if (press_type == LONG_PRESS){
                state = S0_OFF;
            }
            break;
    }
}