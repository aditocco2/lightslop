/*
A smaller piece of the program used to print the symbols onto the LED matrix
during the mode selection screen.

Angelo DiTocco
*/

#include "snake_symbols.h"
#include <led_matrices.h>
#include <symbols.h>
#include <stdint.h>

#define DIGIT_WIDTH 3
#define DIGIT_HEIGHT 5

static uint8_t slow_symbol[8] = {
    ________,
    ________,
    ___X____,
    ____X___,
    ___X____,
    ________,
    ________,
    ________
};

static uint8_t medium_symbol[8] = {
    ________,
    ___X____,
    ____X___,
    _____X__,
    ____X___,
    ___X____,
    ________,
    ________
};

static uint8_t fast_symbol[8] = {
    ________,
    _X__X___,
    __X__X__,
    ___X__X_,
    __X__X__,
    _X__X___,
    ________,
    ________
};

static uint8_t impossible_symbol[8] = {
    ________,
    __XXXX__,
    _X____X_,
    _X_X_XX_,
    _X____X_,
    __X__X__,
    __XXXX__,
    ________
};

static uint8_t snake_logo[8] = {
    ________,
    __XXX___,
    __X_X_X_,
    _XX_X___,
    ____X_X_,
    ____XXX_,
    ________,
    ________
};

void show_slow_symbol(void){
    print_symbol(slow_symbol, 1);
}

void show_medium_symbol(void){
    print_symbol(medium_symbol, 1);
}

void show_fast_symbol(void){
    print_symbol(fast_symbol, 1);
}

void show_impossible_symbol(void){
    print_symbol(impossible_symbol, 1);
}

void show_snake_logo(void){
    print_symbol(snake_logo, 0);
}