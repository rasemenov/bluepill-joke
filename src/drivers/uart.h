#ifndef UART_H
#define UART_H

#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/usart.h>
#include <libopencm3/cm3/nvic.h>

#include <stdbool.h>

void uart_setup(void);

void usart1_stop_transmission(void);

void usart1_trigger_transmission(void);

uint16_t usart1_recv(void);

void usart1_send(uint16_t data);

bool is_read_data_ready(void);

bool is_ready_to_transmit(void);

enum response_state {
    DONE,
    HAS_MORE,
    EMPTY
};

#endif
