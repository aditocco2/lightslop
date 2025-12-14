/*
A smaller piece of the program used to print two-digit numbers to the LED 
matrix using a lookup table.

Angelo DiTocco
*/

#include "symbols.h"
#include "digits.h"
#include <stdint.h>
#include <led_matrices.h>

#define DIGIT_WIDTH 3
#define DIGIT_HEIGHT 5

void compose_two_digits(uint8_t symbol[8], uint8_t tens, uint8_t ones);

static const uint8_t digit_lut[10][DIGIT_HEIGHT] = {
    {
        ______X_,
        _____X_X,
        _____X_X,
        _____X_X,
        ______X_
    },
    {
        ______X_,
        _____XX_,
        ______X_,
        ______X_,
        _____XXX,
    },
    {
        _____XX_,
        _______X,
        ______X_,
        _____X__,
        _____XXX
    },
    {
        _____XX_,
        _______X,
        _____XXX,
        _______X,
        _____XX_
    },
    {
        _____X__,
        _____X_X,
        _____XXX,
        _______X,
        _______X
    },
    {
        _____XXX,
        _____X__,
        _____XXX,
        _______X,
        _____XX_
    },
    {
        ______XX,
        _____X__,
        _____XXX,
        _____X_X,
        _____XXX
    },
    {
        _____XXX,
        _______X,
        ______X_,
        _____X__,
        _____X__
    },
    {
        _____XXX,
        _____X_X,
        _____XXX,
        _____X_X,
        _____XXX
    },
    {
        _____XXX,
        _____X_X,
        _____XXX,
        _______X,
        _____XX_
    }
};

void print_number(uint8_t number, uint8_t matrix_num){
    uint8_t symbol[8];
    uint8_t tens = (number / 10) % 10;
    uint8_t ones = number % 10;
    compose_two_digits(symbol, tens, ones);
    print_symbol(symbol, matrix_num);
}

void compose_two_digits(uint8_t symbol[8], uint8_t tens, uint8_t ones){
    symbol[0] = ________;
    for(uint8_t row = 0; row < DIGIT_HEIGHT; row++){
        uint8_t tens_line = digit_lut[tens][row];
        uint8_t ones_line = digit_lut[ones][row];
        symbol[row + 1] = (tens_line << (DIGIT_WIDTH + 1)) | ones_line;
    }
    symbol[6] = ________;
    symbol[7] = ________;
}