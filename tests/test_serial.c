#include "mocks/Mockuart.h"
#include "mocks/Mockcmds.h"
#include "unity.h"
#include "serial.h"

#include <stdint.h>
#include <string.h>


void setUp(void) {
}


void tearDown(void) {
}

#define BUFFER_TEST 300


void assert_valid_print(char *test_str) {
    uint16_t expected[BUFFER_TEST] = {0};
    int str_len = strlen(test_str);
    for (int indx = 0; indx < str_len; indx++) {
        expected[indx] = (uint16_t)test_str[indx];
        usart1_send_Expect(expected[indx]);
        uart_send_response();
    }
    uint16_t *send_buf = get_send_buffer();
    TEST_ASSERT_EQUAL_UINT16_ARRAY(expected, send_buf, (size_t)str_len);
}


void assert_valid_echo(char character) {
    uart_rcv_echo_buffer(character);
    usart1_send_Expect(character);
    uart_send_echo_buffer();
}


void assert_valid_echo_buffer(uint16_t *expected) {
    uint16_t send_buf[BUFFER_TEST] = {0};
    TEST_ASSERT_NOT_NULL(get_rcv_buffer(send_buf));
    TEST_ASSERT_EQUAL_UINT16_ARRAY(expected, send_buf, ARRAY_LENGTH(send_buf));
}


static char valid_lines[][BUFFER_TEST] = {
    "foo",
    "really long line with whitespaces",
    "11111111111111111111111111111111111111111111111111111111111111111111111"
    "11111111111111111111111111111111111111111111111111111111111111111111111"
    "11111111111111111111111111111111111111111111111111111111111111111111111"
    "1111111111111111111111111111111111111111111"
};


// Check the expected function calls with the test data.
void test_printing_raw_string(void) {
    for (int tc = 0; tc < ARRAY_LENGTH(valid_lines); tc++) {
        usart1_trigger_transmission_Expect();
        uart_put_raw_line(valid_lines[tc]);
        assert_valid_print(valid_lines[tc]);
    }
}


// Just checks the expected function calls with the expected "\r\n".
void test_printing_crlf(void) {
    usart1_trigger_transmission_Expect();
    uart_put_new_line();
    assert_valid_print(CRLF);
}


// Code ignores too long lines. So the state of the output buffer should
// be the same before and after the print call.
void test_printing_too_long_string(void) {
    char invalid_line[BUFFER_TEST];
    uint16_t *old_buffer = get_send_buffer();
    memset(invalid_line, '1', sizeof(invalid_line));
    usart1_trigger_transmission_Expect();
    uart_put_raw_line(invalid_line);
    uint16_t *new_buffer = get_send_buffer();
    TEST_ASSERT_EQUAL_UINT16_ARRAY(old_buffer, new_buffer, ARRAY_LENGTH(old_buffer));
}


void test_uart_echo(void) {
    for (int tc = 0; tc < ARRAY_LENGTH(valid_lines); tc++) {
        char *input = valid_lines[tc];
        int input_len = strlen(input);
        for (int indx = 0; indx < strlen(input); indx++) {
            assert_valid_echo(input[indx]);
        }
    }
}


void test_backspace_no_previous_input(void) {
    uint16_t old_buffer[BUFFER_TEST] = {0};
    get_rcv_buffer(old_buffer);
    for (int i = 0; i < 100; i++) {
        usart1_trigger_transmission_Expect();
        uart_rcv_echo_buffer(0x7f);
        assert_valid_print("\b \b");
    }
    assert_valid_echo_buffer(old_buffer);
}


void test_backspace_existing_words(void) {
    uint16_t old_buffer[BUFFER_TEST] = {0};
    const char *test_str = "foo";
    int indx = 0, str_len = strlen(test_str);
    for (indx = 0; indx < str_len; indx++) {
        assert_valid_echo(test_str[indx]);
    }
    get_rcv_buffer(old_buffer);
    for (indx = 0; indx < str_len; indx++) {
        usart1_trigger_transmission_Expect();
        uart_rcv_echo_buffer(0x7f);
        assert_valid_print("\b \b");
    }
    assert_valid_echo_buffer(old_buffer);
}


int main(void) {
    UnityBegin("tests/test_serial.c");
    RUN_TEST(test_printing_raw_string);
    RUN_TEST(test_printing_crlf);
    RUN_TEST(test_printing_too_long_string);
    RUN_TEST(test_uart_echo);
    RUN_TEST(test_backspace_no_previous_input);
    RUN_TEST(test_backspace_existing_words);
    return UnityEnd();
}
