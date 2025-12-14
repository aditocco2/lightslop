/*
This is a DMA-based driver to control multiple MAX7219 LED matrix modules. It
supports setting/clearing individual pixels and copying over large collections
of pixels using DMA. Interrupts from the timer peripheral are used to
automatically update the display at a configurable refresh rate.

Angelo DiTocco
*/

#include "led_matrices.h"
#include "dma.h"
#include "timer0.h"
#include <rp2350/dma.h>
#include <rp2350/resets.h>
#include <rp2350/sio.h>
#include <rp2350/io_bank0.h>
#include <rp2350/pads_bank0.h>
#include <stdint.h>

#define DATA 13
#define CS 14
#define CLK 15

#define NUM_MATRICES 2

#define BRIGHTNESS 3

#define REFRESH_RATE_HZ 30

#define ROWS (8 * NUM_MATRICES)

#define REFRESH_TIME_US (1000000 / REFRESH_RATE_HZ)

#define NO_DECODE 0
#define SCAN_LIMIT 7
#define NORMAL_OPERATION 1
#define TEST_MODE_OFF 0

#define DECODE_MODE_REG 9
#define BRIGHTNESS_REG 10
#define SCAN_LIMIT_REG 11
#define SHUTDOWN_REG 12
#define DISPLAY_TEST_REG 15

#define CONCAT2(X,Y) X ## Y
#define CONCAT3(X,Y,Z) X ## Y ## Z

#define IO_BANK0_GPIO_CTRL(X) CONCAT3(IO_BANK0_GPIO,X,_CTRL)
#define PADS_BANK0_GPIO(X) CONCAT2(PADS_BANK0_GPIO,X)

#define GPIO_RESETS (RESETS_RESET_IO_BANK0_MASK | RESETS_RESET_PADS_BANK0_MASK)

void set_pixel(uint8_t column, uint8_t row, uint8_t matrix_num);
void clear_pixel(uint8_t column, uint8_t row, uint8_t matrix_num);
uint8_t get_pixel(uint8_t column, uint8_t row, uint8_t matrix_num);

void print_symbol(uint8_t symbol[8], uint8_t matrix_num);
void clear_rows(uint8_t top, uint8_t bottom);
void get_rows(uint8_t return_rows[], uint8_t top, uint8_t bottom);
void clear_matrix(uint8_t matrix_num);
void clear_screen(void);

void update_display(void);

static void send_data_to_reg(uint8_t addr, uint8_t data);
static void send_byte(uint8_t byte);
static void load_data(void);

static void set_pin_to_1(uint8_t pin);
static void set_pin_to_0(uint8_t pin);

static uint8_t pixel_buffer[ROWS];

void configure_led_matrices(void){

    configure_dma();
    configure_timer0();
    
    RESETS_RESET_CLR = GPIO_RESETS;
    while((RESETS_RESET_DONE & GPIO_RESETS ) != GPIO_RESETS )
        continue;

    uint32_t pads_bank0_config_value =
        PADS_BANK0_GPIO0_OD(0)
        | PADS_BANK0_GPIO0_IE(0)
        | PADS_BANK0_GPIO0_DRIVE(0)
        | PADS_BANK0_GPIO0_PUE(1)
        | PADS_BANK0_GPIO0_PDE(0)
        | PADS_BANK0_GPIO0_SCHMITT(0)
        | PADS_BANK0_GPIO0_SLEWFAST(0);

    uint32_t io_bank0_config_value =
        IO_BANK0_GPIO0_CTRL_IRQOVER(0)
        | IO_BANK0_GPIO0_CTRL_INOVER(0)
        | IO_BANK0_GPIO0_CTRL_OEOVER(0)
        | IO_BANK0_GPIO0_CTRL_OUTOVER(0)
        | IO_BANK0_GPIO0_CTRL_FUNCSEL(5);

    PADS_BANK0_GPIO(DATA) = pads_bank0_config_value;
    IO_BANK0_GPIO_CTRL(DATA) = io_bank0_config_value;
    PADS_BANK0_GPIO(CS) = pads_bank0_config_value;
    IO_BANK0_GPIO_CTRL(CS) = io_bank0_config_value;
    PADS_BANK0_GPIO(CLK) = pads_bank0_config_value;
    IO_BANK0_GPIO_CTRL(CLK) = io_bank0_config_value;

    SIO_GPIO_OE_SET = (1 << DATA);
    SIO_GPIO_OE_SET = (1 << CS);
    SIO_GPIO_OE_SET = (1 << CLK);

    set_pin_to_0(CS);
    set_pin_to_0(CLK);
    set_pin_to_0(DATA);

    for(int8_t m = NUM_MATRICES - 1; m >= 0; m++){
        send_data_to_reg(BRIGHTNESS_REG, BRIGHTNESS);
    }
    load_data();
    for(int8_t m = NUM_MATRICES - 1; m >= 0; m++){
        send_data_to_reg(DECODE_MODE_REG, NO_DECODE);
    }
    load_data();
    for(int8_t m = NUM_MATRICES - 1; m >= 0; m++){
        send_data_to_reg(SCAN_LIMIT_REG, SCAN_LIMIT);
    }
    load_data();
    for(int8_t m = NUM_MATRICES - 1; m >= 0; m++){
        send_data_to_reg(DISPLAY_TEST_REG, TEST_MODE_OFF);
    }
    load_data();
    for(int8_t m = NUM_MATRICES - 1; m >= 0; m++){
        send_data_to_reg(SHUTDOWN_REG, NORMAL_OPERATION);
    }
    load_data();
    
    update_display();
}

void set_pixel(uint8_t column, uint8_t row, uint8_t matrix_num){
    uint8_t index = 8 * matrix_num + row;
    uint8_t bit = 8 - 1 - column;
    pixel_buffer[index] |= (1 << bit);
}

void clear_pixel(uint8_t column, uint8_t row, uint8_t matrix_num){
    uint8_t index = 8 * matrix_num + row;
    uint8_t bit = 8 - 1 - column;
    pixel_buffer[index] &= ~(1 << bit);
}

uint8_t get_pixel(uint8_t column, uint8_t row, uint8_t matrix_num){
    uint8_t index = 8 * matrix_num + row;
    uint8_t bit = 8 - 1 - column;
    return ((pixel_buffer[index] >> (bit)) & 1);
}

void print_symbol(uint8_t symbol[8], uint8_t matrix_num){
    uint8_t *dest = pixel_buffer + 8 * matrix_num;
    uint8_t *src = symbol;
    memcpy8(dest, src, 8);
}

void clear_rows(uint8_t top, uint8_t bottom){
    uint8_t *dest = pixel_buffer + top;
    uint8_t n = bottom + 1 - top;
    clear8(dest, n);
}

void get_rows(uint8_t return_rows[], uint8_t top, uint8_t bottom){
    uint8_t num_rows = bottom + 1 - top;
    uint8_t *dest = return_rows;
    uint8_t *src = pixel_buffer + top;
    memcpy8(dest, src, num_rows);
}

void clear_matrix(uint8_t matrix_num){
    uint8_t top_row = 8 * matrix_num;
    uint8_t bottom_row = 8 * matrix_num + 7;
    clear_rows(top_row, bottom_row);
}

void clear_screen(void){
    clear_rows(0, ROWS - 1);
}

void update_display(void){ 
    set_alarm(0, REFRESH_TIME_US, update_display, 15);

    for(int8_t addr = 1; addr <= 8; addr++){
        for(int8_t m = NUM_MATRICES - 1; m >= 0; m--){
            uint8_t row = pixel_buffer[(m * 8) + (addr - 1)];
            send_data_to_reg(addr, row);
        }
        load_data();
    }
}

static void send_data_to_reg(uint8_t addr, uint8_t data){
    set_pin_to_0(CS);
    send_byte(addr);
    send_byte(data);
}

static void load_data(void){
    set_pin_to_1(CS);
}

static void send_byte(uint8_t byte){
    for(int8_t i = 7; i >= 0; i--){
        if(byte & (1 << i)){
            set_pin_to_1(DATA);
        }
        else{
            set_pin_to_0(DATA); 
        }
        set_pin_to_1(CLK);
        set_pin_to_0(CLK);
    }
}

static void set_pin_to_1(uint8_t pin){
    SIO_GPIO_OUT_SET = (1 << pin);
}

static void set_pin_to_0(uint8_t pin){
    SIO_GPIO_OUT_CLR = (1 << pin);
}