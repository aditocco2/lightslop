/*
This main program uses an FSM to rotate the servo based on keypad input.
The * key moves it left (counterclockwise from top down) and # moves it
right (clockwise from top down). Double taps move it in a small increment
and single taps move it in a larger increment.

Angelo DiTocco
*/

#include <servo.h>
#include <keypad.h>
#include <systick.h>
#include <interrupt.h>
#include <stdint.h>

#define STEPSIZE 100
#define DOUBLE_TAP_INTERVAL_MS 500

#define SERVO_MIN -1000
#define SERVO_MAX 1000

static int16_t servo_pos;
static uint32_t time_since_last_press;
static uint32_t last_press_time;

void move_servo_fsm();
void update_last_press_time();
void move_left(uint16_t units_to_move);
void move_right(uint16_t units_to_move);

void main(void){
    __disable_irq();
    configure_systick();
    configure_servo();
    configure_keypad();
    __enable_irq();

    while(1){
        __wait_for_interrupt();
        if (!system_tick()){
            continue;
        }

        move_servo_fsm();
    }
}

void move_servo_fsm(){
    static char key;
    
    time_since_last_press = get_system_time() - last_press_time;
    task_keypad_scan();
    keypad_getchar(&key);

    static enum {S0_BASE, S1_L_TAP, S2_R_TAP} state;
    switch(state){
        case S0_BASE:
            if (key == '*'){
                state = S1_L_TAP;
                update_last_press_time();
            }
            else if (key == '#'){
                state = S2_R_TAP;
                update_last_press_time();
            }
            break;
        case S1_L_TAP:
            if (key == '*'){
                state = S0_BASE;
                move_left(STEPSIZE/2);
                update_last_press_time();
            }
            else if (key == '#'){
                state = S2_R_TAP;
                move_left(STEPSIZE);
                update_last_press_time();
            }
            else if (time_since_last_press >= DOUBLE_TAP_INTERVAL_MS){
                state = S0_BASE;
                move_left(STEPSIZE);
            }
            break;
        case S2_R_TAP:
            if (key == '#'){
                state = S0_BASE;
                move_right(STEPSIZE/2);
                update_last_press_time();
            }
            else if (key == '*'){
                state = S1_L_TAP;
                move_right(STEPSIZE);
                update_last_press_time();
            }
            else if (time_since_last_press >= DOUBLE_TAP_INTERVAL_MS){
                state = S0_BASE;
                move_right(STEPSIZE);
            }
    }
    
}

void move_right(uint16_t units_to_move){
    servo_pos -= units_to_move;
    if (servo_pos < SERVO_MIN){
        servo_pos = SERVO_MIN;
    }

    rotate_servo(servo_pos);
}

void move_left(uint16_t units_to_move){
    servo_pos += units_to_move;
    if (servo_pos > SERVO_MAX){
        servo_pos = SERVO_MAX;
    }

    rotate_servo(servo_pos);
}

void update_last_press_time(){
    last_press_time = get_system_time();
}