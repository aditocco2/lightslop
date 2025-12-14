#include <led_matrices.h>
#include <interrupt.h>
#include <systick.h>
#include <five_buttons.h>
#include <symbols.h>
#include <stdint.h>

static int8_t digit = 0;

void incr(void);

void main(void){
    __disable_irq();
    configure_systick();
    configure_buttons();
    configure_led_matrices();
    __enable_irq();

    copy_digit(digit, 0, 0);

    while(1){
        __wait_for_interrupt();
        if(!system_tick()){
            continue;
        }

        if(btnU_is_pressed()){
            incr();
        }
        
        update_display();
    }
}

void incr(void){
    digit++;
    if(digit > 9){
        digit = 0;
    }
    copy_digit(digit, 0, 0);
}
