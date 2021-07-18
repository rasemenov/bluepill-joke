#ifndef SERIAL_H
#define SERIAL_H

#include <stdint.h>
#include <stdlib.h>

#define ARRAY_LENGTH(x) ((int)(sizeof(x) / sizeof((x)[0])))

#define RCV_BUFFER_SIZE 128
#define SEND_BUFFER_SIZE 256
#define CRLF "\r\n"

void uart_put_raw_line(const char *line);

void uart_put_line(const char *line);

void uart_put_new_line(void);

enum response_state uart_send_response(void);

void uart_send_echo_buffer(void);

void uart_rcv_echo_buffer(uint16_t data);

void uart_check_for_cmd(void);

#endif
