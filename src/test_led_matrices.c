#include <led_matrices.h>
#include <interrupt.h>
#include <systick.h>
#include <five_buttons.h>
#include <digits.h>
#include <snake_symbols.h>
#include <stdint.h>

static uint8_t score = 0;
static enum {SLOW, MEDIUM, FAST} speed;

void update_score(void);
void mode_select(void);
void conditionally_show_logo(void);

void main(void){
    __disable_irq();
    configure_systick();
    configure_buttons();
    configure_led_matrices();
    __enable_irq();

    show_slow_symbol();
    print_number(score, 0);

    while(1){
        __wait_for_interrupt();
        if(!system_tick()){
            continue;
        }

        update_score();
        mode_select();
        conditionally_show_logo();
    }
}

void update_score(void){
    if(btnU_is_pressed()){
        score++;
        print_number(score, 0);
    }
    if(btnD_is_pressed()){
        score--;
        print_number(score, 0);
    }
}

void mode_select(void){
    if(btnR_is_pressed()){
        switch(speed){
            case SLOW:
                speed = MEDIUM;
                show_medium_symbol();
                break;
            case MEDIUM:
                speed = FAST;
                show_fast_symbol();
                break;
            case FAST:
                speed = SLOW;
                show_slow_symbol();
                break;
        }
    }
}

void conditionally_show_logo(void){
    if(btnL_is_pressed()){
        clear_matrix(1);
        show_snake_logo();
    }
}
