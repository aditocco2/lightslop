#include <led_pio.h>
#include <interrupt.h>
#include <systick.h>

void main(void){
    configure_led_pio();

    while(1){
        __wait_for_interrupt();
        if (!system_tick()){
            continue;
        }
    }
}