/*
The top-level FSM controlling the snake game.

Angelo DiTocco
*/

#include <stdint.h>
#include <stdbool.h>
#include <interrupt.h>
#include <systick.h>
#include <watchdog.h>
#include <dma.h>
#include <led_matrices.h>
#include <five_buttons.h>
#include "digits.h"
#include "snake_symbols.h"
#include "snake_data.h"
#include "snake_buffer.h"
#include "prng.h"

#define SLOW_SPEED_HZ 2
#define MEDIUM_SPEED_HZ 3
#define FAST_SPEED_HZ 5
#define IMPOSSIBLE_SPEED_HZ 10

void snake(void);
void update_direction(direction current_direction, direction *buffered_direction);
_Bool out_of_bounds(coordinate coord);
coordinate generate_food(void);
void reset_timer(uint64_t *start_time);
uint64_t timer_value(uint64_t start_time);

void main(void){
    __disable_irq();
    configure_systick();
    configure_watchdog(1100);
    configure_led_matrices();
    configure_buttons();
    register_systick_callback(snake);
    register_systick_callback(update_prng);
    register_systick_callback(feed_the_watchdog);
    __enable_irq();

    seed_prng(146);

    while(1){
        __wait_for_interrupt();
        if(!system_tick()){
            continue;
        }
    }
}

void snake(void){

    static direction buffered_direction, current_direction;

    static enum {SLOW, MEDIUM, FAST, IMPOSSIBLE} speed;
    static uint16_t game_interval_ms;

    static coordinate food_location;
    static coordinate head_pos, tail_pos;

    static uint8_t score = 0;

    static uint64_t start_time;

    static enum {
        INIT_MODE_SELECT,
        SELECT_SLOW, SELECT_MEDIUM, SELECT_FAST, SELECT_IMPOSSIBLE,
        INIT_GAMEPLAY,
        WAIT, MOVE_SNAKE, COLLISION_CHECK, UPDATE_SCREEN, GENERATE_FOOD,
        GAME_OVER
    } state;

    switch(state){
        case INIT_MODE_SELECT:
            show_snake_logo();
            if(speed == SLOW){
                state = SELECT_SLOW;
            }
            else if(speed == MEDIUM){
                state = SELECT_MEDIUM;
            }
            else if(speed == FAST){
                state = SELECT_FAST;
            }
            else if(speed == IMPOSSIBLE){
                state = SELECT_IMPOSSIBLE;
            }
            break;

        case SELECT_SLOW:
            speed = SLOW;
            show_slow_symbol();
            game_interval_ms = 1000 / SLOW_SPEED_HZ;
            if(btnR_is_pressed()){
                state = SELECT_MEDIUM;
            }
            if(btnC_is_pressed()){
                state = INIT_GAMEPLAY;
            }
            break;

        case SELECT_MEDIUM:
            speed = MEDIUM;
            show_medium_symbol();
            game_interval_ms = 1000 / MEDIUM_SPEED_HZ;
            if(btnR_is_pressed()){
                state = SELECT_FAST;
            }
            else if(btnL_is_pressed()){
                state = SELECT_SLOW;
            }
            if(btnC_is_pressed()){
                state = INIT_GAMEPLAY;
            }
            break;

        case SELECT_FAST:
            speed = FAST;
            game_interval_ms = 1000 / FAST_SPEED_HZ;
            show_fast_symbol();
            if(btnR_is_pressed()){
                state = SELECT_IMPOSSIBLE;
            }
            else if(btnL_is_pressed()){
                state = SELECT_MEDIUM;
            }
            if(btnC_is_pressed()){
                state = INIT_GAMEPLAY;
            }
            break;

        case SELECT_IMPOSSIBLE:
            speed = IMPOSSIBLE;
            game_interval_ms = 1000 / IMPOSSIBLE_SPEED_HZ;
            show_impossible_symbol();
            if(btnL_is_pressed()){
                state = SELECT_FAST;
            }    
            if(btnC_is_pressed()){
                state = INIT_GAMEPLAY;
            }
            break;

        case INIT_GAMEPLAY:
            clear_screen();

            reset_snake_buffer();
            head_pos.x = 3;
            head_pos.y = 7;
            write_to_snake_buffer(head_pos);
            set_pixel(head_pos.x, head_pos.y, 1);

            current_direction = UP;
            buffered_direction = UP;

            food_location = generate_food();

            score = 0;
            print_number(score, 0);

            reset_timer(&start_time);
            state = WAIT;
            break;
        
        case WAIT:
            update_direction(current_direction, &buffered_direction);
            if(timer_value(start_time) >= game_interval_ms){
                state = MOVE_SNAKE;
                reset_timer(&start_time);
            }
            break;
        
        case MOVE_SNAKE:
            update_direction(current_direction, &buffered_direction);
            current_direction = buffered_direction;

            switch(current_direction){
                case UP:
                    head_pos.y--;
                    break;
                case DOWN:
                    head_pos.y++;
                    break;
                case LEFT:
                    head_pos.x--;
                    break;
                case RIGHT:
                    head_pos.x++;
                    break;
            }

            write_to_snake_buffer(head_pos);

            if(!coordinates_equal(head_pos, food_location)){
                tail_pos = read_from_snake_buffer();
            }

            state = COLLISION_CHECK;
            break;

            case COLLISION_CHECK:
                update_direction(current_direction, &buffered_direction);
                if(collision() || out_of_bounds(head_pos)){
                    reset_timer(&start_time);
                    state = GAME_OVER;
                }
                else if (snake_buffer_is_full()){
                    score++;
                    print_number(score, 0);
                    reset_timer(&start_time);
                    state = GAME_OVER;
                }
                else if (coordinates_equal(head_pos, food_location)){
                    score++;
                    print_number(score, 0);
                    state = GENERATE_FOOD;
                }
                else {
                    state = UPDATE_SCREEN;
                }
                break;

            case UPDATE_SCREEN:
                update_direction(current_direction, &buffered_direction);
                clear_pixel(tail_pos.x, tail_pos.y, 1);
                set_pixel(head_pos.x, head_pos.y, 1);
                state = WAIT; 
                break;

            case GENERATE_FOOD:
                update_direction(current_direction, &buffered_direction);
                food_location = generate_food();
                state = WAIT;
                break;
            
            case GAME_OVER:
                if ((timer_value(start_time) >= 1000) && btnC_is_pressed()){
                    state = INIT_MODE_SELECT;
                }
    }
}

void update_direction(direction current_direction, direction *buffered_direction){
    if(btnU_is_pressed() && current_direction != DOWN){
        *buffered_direction = UP;
    }
    if(btnD_is_pressed() && current_direction != UP){
        *buffered_direction = DOWN;
    }
    if(btnL_is_pressed() && current_direction != RIGHT){
        *buffered_direction = LEFT;
    }
    if(btnR_is_pressed() && current_direction != LEFT){
        *buffered_direction = RIGHT;
    }
}

_Bool out_of_bounds(coordinate coord){
    if(coord.x > 7 || coord.y > 7){
        return true;
    }
    else{
        return false;
    }
}

coordinate generate_food(void){
    uint8_t empty_spaces = get_empty_spaces();

    uint16_t prng_middle_16 = (get_prng() >> 8) & 0xFFFF;
    uint8_t down_count = prng_middle_16 % empty_spaces + 1;
    uint8_t temp_x = 0, temp_y = 0;
    while(down_count > 0){
        if(get_pixel(temp_x, temp_y, 1) == 0){
            down_count--;
        }

        if(down_count == 0){
            break;
        }

        temp_x++;
        if(temp_x >= 8){
            temp_y++;
            temp_x = 0;
        }
    }

    coordinate food_location;
    food_location.x = temp_x;
    food_location.y = temp_y;

    set_pixel(food_location.x, food_location.y, 1);
    return food_location;
}

void reset_timer(uint64_t *start_time){
    *start_time = get_system_time();
}

uint64_t timer_value(uint64_t start_time){
    return get_system_time() - start_time;
}