#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>

#include "sys/serial.h"
#include "sys/timer.h"
#include "drivers/uart.h"


int main(void) {
	rcc_clock_setup_in_hse_8mhz_out_72mhz(); // Blue pill
    set_systick();

	// PC13:
	rcc_periph_clock_enable(RCC_GPIOC);
	gpio_set_mode(GPIOC, GPIO_MODE_OUTPUT_2_MHZ, GPIO_CNF_OUTPUT_PUSHPULL,
                  GPIO13);

	uart_setup();
    uart_put_raw_line("Welcome!" CRLF "# ");
    for (;;) {
        uart_check_for_cmd();
    }
	return 0;
}
