#include "uart.h"


void uart_setup(void) {
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

	usart_set_baudrate(USART1,115200);
	usart_set_databits(USART1,8);
	usart_set_stopbits(USART1,USART_STOPBITS_1);
	usart_set_mode(USART1,USART_MODE_TX_RX);
	usart_set_parity(USART1,USART_PARITY_NONE);
	usart_set_flow_control(USART1,USART_FLOWCONTROL_NONE);
    USART_CR1(USART1) |= USART_CR1_RXNEIE;
	usart_enable(USART1);
}


void usart1_trigger_transmission(void) {
    USART_CR1(USART1) |= USART_CR1_TXEIE;
}


void usart1_stop_transmission(void) {
    USART_CR1(USART1) &= ~USART_CR1_TXEIE;
}


void usart1_send(uint16_t data) {
    usart_send(USART1, data);
}


uint16_t usart1_recv(void) {
    return usart_recv(USART1);
}

bool is_read_data_ready(void) {
    return (bool)(USART_SR(USART1) & USART_SR_RXNE);
}


bool is_ready_to_transmit(void) {
    return (bool)(USART_SR(USART1) & USART_SR_TXE);
}
