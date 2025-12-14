/*
This driver is used to read the input from the numeric keypad. An FSM scans for
any keys pressed and writes the result to the variable keypad_char. A getter
function then picks up this character so it can be used externally.

Angelo DiTocco
*/

#include "keypad.h"
#include <rp2350/resets.h>
#include <rp2350/sio.h>
#include <rp2350/io_bank0.h>
#include <rp2350/pads_bank0.h>
#include <stdint.h>

#define DEBOUNCE_TIME_MS 10

#define ROW0 13
#define ROW1 12
#define ROW2 11
#define ROW3 10
#define COL0 9
#define COL1 8
#define COL2 7

#define ALL 15
#define NO_KEY -1

#define CONCAT2(X,Y) X ## Y
#define CONCAT3(X,Y,Z) X ## Y ## Z
#define IO_BANK0_GPIO_CTRL(X) CONCAT3(IO_BANK0_GPIO,X,_CTRL)
#define PADS_BANK0_GPIO(X) CONCAT2(PADS_BANK0_GPIO,X)
#define GPIO_RESETS (RESETS_RESET_IO_BANK0_MASK | RESETS_RESET_PADS_BANK0_MASK)

static char keypad_char;

void configure_keypad();
void task_keypad_scan();
int keypad_getchar(char *c);

static void scan_row(uint8_t row);
static int8_t read_col();

static void set_pin_to_0(uint8_t pin);
static void set_pin_to_z(uint8_t pin);
static void take_pin_out_of_z(uint8_t pin);
static _Bool read_pin(uint8_t pin);
static void set_all_rows_to_0();
static void set_all_rows_to_z();
static void take_all_rows_out_of_z();

void configure_keypad(void){
    keypad_char = 0;

    RESETS_RESET_CLR = GPIO_RESETS;
    while((RESETS_RESET_DONE & GPIO_RESETS ) != GPIO_RESETS )
        continue;

    uint32_t pads_bank0_row_config_value =
        PADS_BANK0_GPIO0_OD(0)
        | PADS_BANK0_GPIO0_IE(0)
        | PADS_BANK0_GPIO0_DRIVE(0)
        | PADS_BANK0_GPIO0_PUE(0)
        | PADS_BANK0_GPIO0_PDE(0)
        | PADS_BANK0_GPIO0_SCHMITT(0)
        | PADS_BANK0_GPIO0_SLEWFAST(0);

    uint32_t io_bank0_row_config_value =
        IO_BANK0_GPIO0_CTRL_IRQOVER(0)
        | IO_BANK0_GPIO0_CTRL_INOVER(0)
        | IO_BANK0_GPIO0_CTRL_OEOVER(0)
        | IO_BANK0_GPIO0_CTRL_OUTOVER(0)
        | IO_BANK0_GPIO0_CTRL_FUNCSEL(5);

    PADS_BANK0_GPIO(ROW0) = pads_bank0_row_config_value;
    IO_BANK0_GPIO_CTRL(ROW0) = io_bank0_row_config_value;
    PADS_BANK0_GPIO(ROW1) = pads_bank0_row_config_value;
    IO_BANK0_GPIO_CTRL(ROW1) = io_bank0_row_config_value;
    PADS_BANK0_GPIO(ROW2) = pads_bank0_row_config_value;
    IO_BANK0_GPIO_CTRL(ROW2) = io_bank0_row_config_value;
    PADS_BANK0_GPIO(ROW3) = pads_bank0_row_config_value;
    IO_BANK0_GPIO_CTRL(ROW3) = io_bank0_row_config_value;

    uint32_t pads_bank0_col_config_value =
        PADS_BANK0_GPIO0_OD(1)
        | PADS_BANK0_GPIO0_IE(1)
        | PADS_BANK0_GPIO0_DRIVE(0)
        | PADS_BANK0_GPIO0_PUE(1)
        | PADS_BANK0_GPIO0_PDE(0)
        | PADS_BANK0_GPIO0_SCHMITT(0)
        | PADS_BANK0_GPIO0_SLEWFAST(0);

    uint32_t io_bank0_col_config_value =
        IO_BANK0_GPIO0_CTRL_IRQOVER(0)
        | IO_BANK0_GPIO0_CTRL_INOVER(0)
        | IO_BANK0_GPIO0_CTRL_OEOVER(0)
        | IO_BANK0_GPIO0_CTRL_OUTOVER(0)
        | IO_BANK0_GPIO0_CTRL_FUNCSEL(5);
    
    PADS_BANK0_GPIO(COL0) = pads_bank0_col_config_value;
    IO_BANK0_GPIO_CTRL(COL0) = io_bank0_col_config_value;
    PADS_BANK0_GPIO(COL1) = pads_bank0_col_config_value;
    IO_BANK0_GPIO_CTRL(COL1) = io_bank0_col_config_value;
    PADS_BANK0_GPIO(COL2) = pads_bank0_col_config_value;
    IO_BANK0_GPIO_CTRL(COL2) = io_bank0_col_config_value;

    scan_row(ALL);
}

static void scan_row(uint8_t row){
    set_all_rows_to_0();
    set_all_rows_to_z();

    switch(row) {
        case 0:
            take_pin_out_of_z(ROW0);
            break;
        case 1:
            take_pin_out_of_z(ROW1);
            break;
        case 2:
            take_pin_out_of_z(ROW2);
            break;
        case 3:
            take_pin_out_of_z(ROW3);
            break;
        case ALL:
            take_all_rows_out_of_z();
            break;
        default:
            break;
    }
}

static int8_t read_col(){
    int8_t retval = NO_KEY;
    if(read_pin(COL0) == 0){
        retval = 0;   
    }
    else if(read_pin(COL1) == 0){
        retval = 1;
    }
    else if(read_pin(COL2) == 0){
        retval = 2;
    }
    return retval;
}

void task_keypad_scan(){
    static enum {S0_WAIT_PRESS, S1_SCAN0, S2_SCAN1, S3_SCAN2, S4_SCAN3, S5_WAIT_RELEASE} state;
    
    int8_t key_pos = NO_KEY;

    int8_t col = read_col();               
    static uint8_t debounce_cntr = 0;

    switch (state){
        case S0_WAIT_PRESS:
            if (col != NO_KEY){
                state = S1_SCAN0;
                scan_row(0);
            }
            break;
        
        case S1_SCAN0:
            if (col != NO_KEY){
                state = S5_WAIT_RELEASE;
                scan_row(ALL);
                debounce_cntr = 0;
                key_pos = 0 + col;
            }
            else {
                state = S2_SCAN1;
                scan_row(1);
            }
            break;
        case S2_SCAN1:
            if (col != NO_KEY){
                state = S5_WAIT_RELEASE;
                scan_row(ALL);
                debounce_cntr = 0;
                key_pos = 3 + col;
            }
            else{
                state = S3_SCAN2;
                scan_row(2);
            }
            break;
        case S3_SCAN2:
            if (col != NO_KEY){
                state = S5_WAIT_RELEASE;
                scan_row(ALL);
                debounce_cntr = 0;
                key_pos = 6 + col;
            }
            else{
                state = S4_SCAN3;
                scan_row(3);
            }
            break;
        case S4_SCAN3:
            state = S5_WAIT_RELEASE;
            debounce_cntr = 0;
            scan_row(ALL);
            if (col != NO_KEY){
                key_pos = 9 + col;
            }
            break;

        case S5_WAIT_RELEASE:
            if (col == NO_KEY){
                debounce_cntr++;
                if (debounce_cntr >= DEBOUNCE_TIME_MS){
                    state = S0_WAIT_PRESS;
                    scan_row(ALL);
                }
            }
            else{
                debounce_cntr = 0;
            }
            break;
    }

    char keypad_char_lut[] = {'1', '2', '3',
                              '4', '5', '6',
                              '7', '8', '9',
                              '*', '0', '#'};

    if (key_pos != NO_KEY){
        keypad_char = keypad_char_lut[key_pos];
    }
}

int keypad_getchar(char *c){
    *c = keypad_char;
    if (keypad_char != 0) {
        keypad_char = 0;
        return 1;
    }
    else{
        return 0;
    }
}

static void set_pin_to_0(uint8_t pin){
    SIO_GPIO_OUT_CLR = (1 << pin);
    SIO_GPIO_OE_SET = (1 << pin);
}

static void set_pin_to_z(uint8_t pin){
    SIO_GPIO_OE_CLR = (1 << pin);
}

static void take_pin_out_of_z(uint8_t pin){
    SIO_GPIO_OE_SET = (1 << pin);
}

static void set_all_rows_to_0(){
    uint8_t row_pins[] = {ROW0, ROW1, ROW2, ROW3};
    for (uint8_t i = 0; i < 4; i++){
        set_pin_to_0(row_pins[i]);
    }
}

static void set_all_rows_to_z(){
    uint8_t row_pins[] = {ROW0, ROW1, ROW2, ROW3};
    for (uint8_t i = 0; i < 4; i++){
        set_pin_to_z(row_pins[i]);
    }
}

static void take_all_rows_out_of_z(){
    uint8_t row_pins[] = {ROW0, ROW1, ROW2, ROW3};
    for (uint8_t i = 0; i < 4; i++){
        take_pin_out_of_z(row_pins[i]);
    }
}

static _Bool read_pin(uint8_t pin){
    return (SIO_GPIO_IN & (1 << pin));
}