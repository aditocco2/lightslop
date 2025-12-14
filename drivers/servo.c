/*
This driver controls the servo on pin 0 using PWM. Its primary
function is the rotate_servo function, which allows it to be moved
to a position ranging from -1000 to 1000.

Angelo DiTocco
*/

#include "servo.h"
#include <rp2350/resets.h>
#include <rp2350/sio.h>
#include <rp2350/io_bank0.h>
#include <rp2350/pads_bank0.h>
#include <rp2350/pwm.h>
#include <stdint.h>

#define SERVO_LOC 5
//The PWM channel used is the integer part of (SERVO_LOC % 16) / 2
#define CHANNEL 2

#define SERVO_OFFSET 0
#define SERVO_MIN (500 + SERVO_OFFSET)
#define SERVO_ZERO (1500 + SERVO_OFFSET)
#define SERVO_MAX (2500 + SERVO_OFFSET)

#define CONCAT2(X,Y) X ## Y
#define CONCAT3(X,Y,Z) X ## Y ## Z

#define IO_BANK0_GPIO_CTRL(X) CONCAT3(IO_BANK0_GPIO,X,_CTRL)
#define PADS_BANK0_GPIO(X) CONCAT2(PADS_BANK0_GPIO,X)

#define PWM_CH_CC(X) CONCAT3(PWM_CH,X,_CC)
#define PWM_CH_DIV(X) CONCAT3(PWM_CH,X,_DIV)
#define PWM_CH_TOP(X) CONCAT3(PWM_CH,X,_TOP)
#define PWM_CH_CSR(X) CONCAT3(PWM_CH,X,_CSR)

#define SERVO_RESETS (RESETS_RESET_IO_BANK0_MASK \
                        | RESETS_RESET_PADS_BANK0_MASK \
                        | RESETS_RESET_PWM_MASK )

void configure_servo(void);
void rotate_servo(int16_t rotate);

void configure_servo(void){
    RESETS_RESET_CLR = SERVO_RESETS;
    while ((RESETS_RESET_DONE & SERVO_RESETS) != SERVO_RESETS){
        continue;
    }

    PADS_BANK0_GPIO(SERVO_LOC) =
        PADS_BANK0_GPIO0_OD(0)
        | PADS_BANK0_GPIO0_IE(0)
        | PADS_BANK0_GPIO0_DRIVE(0)
        | PADS_BANK0_GPIO0_PUE(1)
        | PADS_BANK0_GPIO0_PDE(0)
        | PADS_BANK0_GPIO0_SCHMITT(0)
        | PADS_BANK0_GPIO0_SLEWFAST(0);

    IO_BANK0_GPIO_CTRL(SERVO_LOC) =
        IO_BANK0_GPIO0_CTRL_IRQOVER(0)
        | IO_BANK0_GPIO0_CTRL_INOVER(0)
        | IO_BANK0_GPIO0_CTRL_OEOVER(0)
        | IO_BANK0_GPIO0_CTRL_OUTOVER(0)
        | IO_BANK0_GPIO0_CTRL_FUNCSEL(4);

    PWM_CH_DIV(CHANNEL) = PWM_CH0_DIV_INT(144) | PWM_CH0_DIV_FRAC(0);
    PWM_CH_TOP(CHANNEL) = 49999;

    rotate_servo(0);
    
    PWM_CH_CSR(CHANNEL) =
        PWM_CH0_CSR_PH_ADV(0)
        |PWM_CH0_CSR_PH_RET(0)
        |PWM_CH0_CSR_DIVMODE(0)
        |PWM_CH0_CSR_B_INV(0)
        |PWM_CH0_CSR_A_INV(0)
        |PWM_CH0_CSR_PH_CORRECT(0)
        |PWM_CH0_CSR_EN(1);
}

void rotate_servo(int16_t rotate){
    int32_t duty = SERVO_ZERO + rotate;

    if (duty > SERVO_MAX){
        duty = SERVO_MAX;
    }
    else if (duty < SERVO_MIN){
        duty = SERVO_MIN;
    }

    if (SERVO_LOC % 2 == 0){  
        PWM_CH_CC(CHANNEL) = PWM_CH0_CC_A(duty);
    }
    else{
        PWM_CH_CC(CHANNEL) = PWM_CH0_CC_B(duty);
    }
}