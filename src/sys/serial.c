#include "cmds.h"
#include "serial.h"
#include "drivers/uart.h"

#include <string.h>
#include <stdbool.h>


uint16_t rcv_buf[RCV_BUFFER_SIZE] = {0};
uint16_t send_buf[SEND_BUFFER_SIZE] = {0};

volatile int buf_indx_write = 0;
volatile int rcv_data_len = 0;
volatile int cmd_data_len = 0;
volatile int send_data_len = 0;
volatile int buf_indx_send = 0;
bool is_cmd_received = false;


void usart1_isr(void) {
    if (is_read_data_ready()) {
        uart_rcv_echo_buffer(usart1_recv());
        usart1_trigger_transmission();
    }

	if (is_ready_to_transmit()) {
        enum response_state state = uart_send_response();
        switch (state) {
            case DONE:
                usart1_stop_transmission();
            case HAS_MORE:
                return;
            case EMPTY:
            default:
                uart_send_echo_buffer();
                /* Disable the TXE interrupt as we don't need it anymore. */
                usart1_stop_transmission();
                break;
        }
	}
}


static void put_data_rcv_buffer(uint16_t data) {
    rcv_buf[buf_indx_write] = data;
    buf_indx_write = (buf_indx_write + 1) % RCV_BUFFER_SIZE;
    rcv_data_len++;
    cmd_data_len++;
}


static void pop_data_rcv_buffer(void) {
    if (buf_indx_write > 0) {
        buf_indx_write--;
    } else {
        buf_indx_write = RCV_BUFFER_SIZE - 1;
    }
    if (rcv_data_len) {
        rcv_data_len--;
    }
    if (cmd_data_len) {
        cmd_data_len--;
    }
}


static int get_read_indx_rcv_buffer(int data_len) {
    int indx_read = buf_indx_write - data_len;
    if (indx_read < 0) {
        indx_read += RCV_BUFFER_SIZE - 1;
    }
    return indx_read;
}


void uart_put_line(const char *line) {
    uart_put_raw_line(line);
    uart_put_new_line();
}


void uart_put_raw_line(const char *line) {
    while (send_data_len);
    size_t len = strlen(line);
    if (len > SEND_BUFFER_SIZE) {
        return;
    }
    memset(send_buf, 0, SEND_BUFFER_SIZE);
    for (size_t indx = 0; indx < len; indx++) {
        send_buf[indx] = line[indx];
    }
    send_data_len = len;
    usart1_trigger_transmission();
}


void uart_put_new_line(void) {
    uart_put_raw_line(CRLF);
}


enum response_state uart_send_response(void) {
    if (send_data_len) {
        /* Allegedly this should clear TXE flag but it does not.*/
        usart1_send(send_buf[buf_indx_send++]);
        send_data_len--;
        if (send_data_len == 0) {
            buf_indx_send = 0;
            return DONE;
        }
        return HAS_MORE;
    }
    return EMPTY;
}


void uart_send_echo_buffer(void) {
    if (rcv_data_len <= 0) {
        return;
    }
    int indx_read = get_read_indx_rcv_buffer(rcv_data_len);
    usart1_send(rcv_buf[indx_read]);
    rcv_data_len--;
}


void uart_rcv_echo_buffer(uint16_t data) {
    if (data == 0x7f) {
        // On backspace move caret backwards, replace symbol with whitespace
        // and move caret backwards again.
        if (cmd_data_len) {
            uart_put_raw_line("\b \b");
            pop_data_rcv_buffer();
        }
        return;
    } else if (data == '\r') {
        uart_put_raw_line("\r\n");
        is_cmd_received = true;
        return;
    }
    put_data_rcv_buffer(data);
}


void uart_check_for_cmd(void) {
    if (!is_cmd_received) {
        return;
    }

    if (cmd_data_len >= RCV_BUFFER_SIZE - 1) {
        cmd_data_len = 0;
        is_cmd_received = false;
        return;
    }
    char cmd_name[RCV_BUFFER_SIZE] = {0};
    int argc = 0, argv_start_indx = 0;
    char *argv[5] = {0};
    for (int indx = 0; indx < cmd_data_len; indx++) {
        int read_indx = get_read_indx_rcv_buffer(cmd_data_len - indx);
        cmd_name[indx] = rcv_buf[read_indx];
        if (cmd_name[indx] == ' ') {
            cmd_name[indx] = '\0';
            argv[argc++] = &cmd_name[argv_start_indx];
            argv_start_indx = indx + 1;
        }
        if (argc == ARRAY_LENGTH(argv)) {
            uart_put_line("Too many arguments");
            break;
        }
    }
    if (argc == 0) {
        cmd_name[cmd_data_len] = '\0';
        argv[argc++] = &cmd_name[argv_start_indx];
    }
    cmd_data_len = 0;
    is_cmd_received = false;
    process_cmd(argc, argv);
    uart_put_raw_line("# ");
}


#ifdef UNIT_TESTS
uint16_t *get_send_buffer(void) {
    return send_buf;
}

uint16_t *get_rcv_buffer(void) {
    return rcv_buf;
}
#endif
