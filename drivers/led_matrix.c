#include "led_matrix.h"
#include <rp2350/resets.h>
#include <rp2350/sio.h>
#include <rp2350/io_bank0.h>
#include <rp2350/pads_bank0.h>
#include <stdint.h>

#define DATA 13
#define CS 14
#define CLK 15

#define BRIGHTNESS 3

#define ROWS 8

#define NO_DECODE 0
#define SCAN_LIMIT 7
#define NORMAL_OPERATION 1
#define TEST_MODE_OFF 0

#define DECODE_MODE_REG 9
#define BRIGHTNESS_REG 10
#define SCAN_LIMIT_REG 11
#define SHUTDOWN_REG 12
#define DISPLAY_TEST_REG 15

#define CONCAT2(X,row) X ## row
#define CONCAT3(X,row,Z) X ## row ## Z

#define IO_BANK0_GPIO_CTRL(X) CONCAT3(IO_BANK0_GPIO,X,_CTRL)
#define PADS_BANK0_GPIO(X) CONCAT2(PADS_BANK0_GPIO,X)

#define GPIO_RESETS (RESETS_RESET_IO_BANK0_MASK | RESETS_RESET_PADS_BANK0_MASK)

void configure_led_matrix(void);
void set_pixel(uint8_t column, uint8_t row);
void clear_pixel(uint8_t column, uint8_t row);
void update_display(void);

static uint8_t pixel_buffer[ROWS];

static void send_data_to_reg(uint8_t addr, uint8_t data);
static void send_byte(uint8_t byte);

static void set_pin_to_1(uint8_t pin);
static void set_pin_to_0(uint8_t pin);

void configure_led_matrix(void){
    
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

    send_data_to_reg(BRIGHTNESS_REG, BRIGHTNESS);
    send_data_to_reg(DECODE_MODE_REG, NO_DECODE);
    send_data_to_reg(SCAN_LIMIT_REG, SCAN_LIMIT);
    send_data_to_reg(DISPLAY_TEST_REG, TEST_MODE_OFF);
    send_data_to_reg(SHUTDOWN_REG, NORMAL_OPERATION);
    
    update_display();
}

void set_pixel(uint8_t column, uint8_t row){
    pixel_buffer[row] |= (1 << (8 - 1 - column));
}

void clear_pixel(uint8_t column, uint8_t row){
    pixel_buffer[row] &= ~(1 << (8 - 1 - column));
}

void update_display(void){
    for(int8_t row = 0; row < 8; row++){
        send_data_to_reg(row + 1, pixel_buffer[row]);
    }
}

static void send_data_to_reg(uint8_t addr, uint8_t data){
    set_pin_to_0(CS);
    send_byte(addr);
    send_byte(data);
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