/*
This is a systick driver that allows up to 5 callbacks to be registered.
Each callback will be executed when systick fires. There is also a function
to get the system time in ticks.

Angelo DiTocco
*/

#include <rp2350/m33.h>
#include <rp2350/ticks.h>
#include <stdbool.h>
#include <stdint.h>
#include <systick.h>
#include <interrupt.h>

#ifndef SYSTICK_FREQ_HZ
#define SYSTICK_FREQ_HZ 1000
#endif

#ifndef EXT_CLK_FREQ_HZ
#define EXT_CLK_FREQ_HZ 1000000
#endif

#ifndef MAX_SYSTICK_CALLBACKS
#define MAX_SYSTICK_CALLBACKS 5
#endif

#define SYSTICK_TOP (EXT_CLK_FREQ_HZ/SYSTICK_FREQ_HZ - 1)

static void (*callbacks[MAX_SYSTICK_CALLBACKS])();
static uint8_t num_callbacks;
static volatile _Bool systick_fired;
static volatile uint64_t tick_cntr;

void configure_systick() {
	TICKS_PROC0_CYCLES = 1;
	TICKS_PROC0_CTRL = TICKS_PROC0_CTRL_ENABLE(1);
	M33_SYST_RVR = SYSTICK_TOP;
	M33_SYST_CVR = 0;
	M33_SYST_CSR = 
		M33_SYST_CSR_CLKSOURCE(0) 
		| M33_SYST_CSR_TICKINT(1) 
		| M33_SYST_CSR_ENABLE(1);
	M33_SHPR3 = (M33_SHPR3 & ~M33_SHPR3_PRI_15_3_MASK )| M33_SHPR3_PRI_15_3(0);
}

_Bool register_systick_callback(void (*cb)()) {
	if (num_callbacks < MAX_SYSTICK_CALLBACKS){
		callbacks[num_callbacks] = cb;
		num_callbacks++;
		return true;
	}
	return false;
}

void __attribute__((interrupt)) SYSTICK_Handler(){
	systick_fired = true;
	tick_cntr++;
	for(uint8_t i=0; i<num_callbacks; i++){
		callbacks[i]();
	}
}

_Bool system_tick() {
	uint32_t irq_status;
	__get_primask(&irq_status);
	__disable_irq();

	_Bool retval = systick_fired;
	systick_fired = false;

	__set_primask(irq_status);

	return retval;
}

uint64_t get_system_time(){
	uint32_t irq_status;
	__get_primask(&irq_status);
	__disable_irq();
	
	uint64_t retval = tick_cntr;
	
	__set_primask(irq_status);

	return retval;
}