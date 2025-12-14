/*
This is a smaller piece of the program controlling the 64-element ring buffer
used to store the pixels occupied by the snake.

Angelo DiTocco
*/

#include "snake_buffer.h"
#include <stdint.h>
#include <stdbool.h>
#include <snake_data.h>

#define SNAKE_BUFFER_SIZE 64
#define next(v) ((v+1) % SNAKE_BUFFER_SIZE)
#define prev(v) (((v-1) + SNAKE_BUFFER_SIZE) % SNAKE_BUFFER_SIZE)

void reset_snake_buffer(void);
void write_to_snake_buffer(coordinate coord);
coordinate read_from_snake_buffer(void);
_Bool snake_buffer_is_full(void);
_Bool collision(void);

static coordinate snake_buffer[SNAKE_BUFFER_SIZE];
static uint8_t head, tail;

void reset_snake_buffer(void){
    head = 0;
    tail = 0;
}

coordinate read_from_snake_buffer(void){
    coordinate coord = snake_buffer[tail];
    tail = next(tail);
    return coord;
}

void write_to_snake_buffer(coordinate coord){
    snake_buffer[head] = coord;
    head = next(head);
}

_Bool snake_buffer_is_full(void){
    return (head == tail);
}

uint8_t get_full_spaces(void){
    return (head - tail + SNAKE_BUFFER_SIZE) % SNAKE_BUFFER_SIZE;
}

uint8_t get_empty_spaces(void){
    return SNAKE_BUFFER_SIZE - get_full_spaces();
}

_Bool collision(void){
    for(uint8_t i = tail; next(i) != head; i = next(i)){
        if(coordinates_equal(snake_buffer[prev(head)], snake_buffer[i])){
            return true;
        }
    }

    return false;
}