#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/usart.h>
#include <libopencm3/cm3/nvic.h>

#include <stdint.h>

#include "sys/serial.h"
#include "sys/timer.h"


extern volatile uint32_t ticks_counter;


void usart1_isr(void) {
    if (USART_SR(USART1) & USART_SR_RXNE) {
        uart_rcv_echo_buffer(usart_recv(USART1));
        USART_CR1(USART1) |= USART_CR1_TXEIE;
    }

	if (USART_SR(USART1) & USART_SR_TXE) {
        enum response_state state = uart_send_response();
        switch (state) {
            case DONE:
                USART_CR1(USART1) &= ~USART_CR1_TXEIE;
            case HAS_MORE:
                return;
            case EMPTY:
            default:
                uart_send_echo_buffer();
                /* Disable the TXE interrupt as we don't need it anymore. */
                USART_CR1(USART1) &= ~USART_CR1_TXEIE;
                break;
        }
	}
}


static void uart_setup(void) {
    nvic_enable_irq(NVIC_USART1_IRQ);
	rcc_periph_clock_enable(RCC_GPIOA);
	rcc_periph_clock_enable(RCC_USART1);

	// UART TX on PA9 (GPIO_USART1_TX)
	gpio_set_mode(GPIOA,
                  GPIO_MODE_OUTPUT_50_MHZ,
                  GPIO_CNF_OUTPUT_ALTFN_PUSHPULL,
                  GPIO_USART1_TX);
	gpio_set_mode(GPIOA,
                  GPIO_MODE_INPUT,
                  GPIO_CNF_INPUT_FLOAT,
                  GPIO_USART1_RX);

	usart_set_baudrate(USART1,38400);
	usart_set_databits(USART1,8);
	usart_set_stopbits(USART1,USART_STOPBITS_1);
	usart_set_mode(USART1,USART_MODE_TX_RX);
	usart_set_parity(USART1,USART_PARITY_NONE);
	usart_set_flow_control(USART1,USART_FLOWCONTROL_NONE);
    USART_CR1(USART1) |= USART_CR1_RXNEIE;
	usart_enable(USART1);
}


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
