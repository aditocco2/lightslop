/*
This driver supports setting up to four alarms using the TIMER0 peripheral.

Angelo DiTocco
*/

#include "timer0.h"
#include <rp2350/resets.h>
#include <rp2350/timer.h>
#include <rp2350/m33.h>
#include <rp2350/ticks.h>
#include <stdbool.h>
#include <stdint.h>
#include "interrupt.h"

static void (*callback[4])();

#define TIMER0_RESETS (RESETS_RESET_TIMER0_MASK)

void configure_timer0(void){
    RESETS_RESET_CLR = TIMER0_RESETS;
    while((RESETS_RESET_DONE & TIMER0_RESETS) != TIMER0_RESETS){
        continue;
    }

    TICKS_TIMER0_CTRL_SET = TICKS_TIMER0_CTRL_ENABLE_MASK;
    TICKS_TIMER0_CYCLES = 1;
    TIMER0_INTE_SET = ((1 << 0) | (1 << 1) | (1 << 2) | (1 << 3));
    M33_NVIC_ISER0 = ((1 << 0) | (1 << 1) | (1 << 2) | (1 << 3));
}

void set_alarm(uint8_t alarm_number, uint32_t time_us,
                void (*pf)(), uint8_t priority_8b){
    if( alarm_number <= 4 ){
        callback[alarm_number] = pf;
        ((uint8_t *)&(M33_NVIC_IPR0))[16+alarm_number]=priority_8b;
        (&(TIMER0_ALARM0))[alarm_number] = TIMER0_TIMELR + time_us;
    }
}

void __attribute__((interrupt)) TIMER0_IRQ_0_Handler(){
    TIMER0_INTR = (1<<0);
    callback[0]();
}
void __attribute__((interrupt)) TIMER0_IRQ_1_Handler(){
    TIMER0_INTR = (1<<1);
    callback[1]();
}
void __attribute__((interrupt)) TIMER0_IRQ_2_Handler(){
    TIMER0_INTR = (1<<2);
    callback[2]();
}
void __attribute__((interrupt)) TIMER0_IRQ_3_Handler(){
    TIMER0_INTR = (1<<3);
    callback[3]();
}