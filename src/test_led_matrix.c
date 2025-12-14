#include <led_matrix.h>
#include <interrupt.h>
#include <systick.h>
#include <five_buttons.h>
#include <stdint.h>

#define SCREEN_WIDTH 8
#define SCREEN_HEIGHT 8

#define X_MAX SCREEN_WIDTH - 1
#define Y_MAX SCREEN_HEIGHT - 1
#define X_MIN 0
#define Y_MIN 0

static int8_t x = 0, y = 0;

void move_left(void);
void move_down(void);
void move_up(void);
void move_right(void);

void main(void){
    __disable_irq();
    configure_systick();
    configure_buttons();
    configure_led_matrix();
    __enable_irq();

    set_pixel(x, y);

    while(1){
        __wait_for_interrupt();
        if (!system_tick()){
            continue;
        }

        clear_pixel(x, y);

        if(btnU_is_pressed()){
            move_up();
        }
        if(btnD_is_pressed()){
            move_down();
        }
        if(btnL_is_pressed()){
            move_left();
        }
        if(btnR_is_pressed()){
            move_right();
        }

        set_pixel(x, y);
        update_display();
    }
}

void move_right(void){
    x++;
    if(x > X_MAX){
        x = X_MAX;
    }
}

void move_left(void){
    x--;
    if(x < X_MIN){
        x = X_MIN;
    }
}

void move_down(void){
    y++;
    if(y > Y_MAX){
        y = Y_MAX;
    }
}

void move_up(void){
    y--;
    if(y < Y_MIN){
        y = Y_MIN;
    }
}
