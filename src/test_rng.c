#include <led_matrices.h>
#include <interrupt.h>
#include <systick.h>
#include <five_buttons.h>
#include <rng.h>
#include <dma.h>
#include <stdint.h>

void test_rng(void);

void main(void){

    __disable_irq();
    configure_led_matrices();
    configure_buttons();
    configure_systick();
    register_systick_callback(update_rng);
    __enable_irq();
    
    seed_rng(146);

    while(1){
        __wait_for_interrupt();
        if(!system_tick()){
            continue;
        }

        if(btnD_is_pressed()){
            test_rng();
        }
    }
}

void test_rng(void){
    uint32_t rng = get_rng();
    uint16_t middle_16 = (rng & (0xFFFF << 16)) >> 16;
    uint8_t random_pixel = middle_16 % 128;
    uint8_t x = random_pixel % 8;
    uint8_t y = random_pixel / 8;
    clear_screen();
    wait_until_dma_is_free();
    set_pixel(x, y);
}

