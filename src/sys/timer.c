#include <libopencm3/cm3/nvic.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/cm3/systick.h>

#include "timer.h"


volatile uint32_t ticks_counter = 0;


void sys_tick_handler(void) {
    ticks_counter++;
}


void set_systick(void) {
    /* AHB / 8 - SysTick frequency
     * 1000 - for interrupt every 1ms
     */
    uint32_t systick_reload = rcc_ahb_frequency / 8000;

    systick_set_clocksource(STK_CSR_CLKSOURCE_AHB_DIV8);
    systick_set_reload(systick_reload);
    systick_interrupt_enable();
    systick_counter_enable();
}
