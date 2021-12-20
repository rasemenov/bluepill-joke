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

#define SEND_BUFFER_TEST 300

static char valid_lines[][SEND_BUFFER_TEST] = {
    "foo",
    "really long line with whitespaces",
    "11111111111111111111111111111111111111111111111111111111111111111111111"
    "11111111111111111111111111111111111111111111111111111111111111111111111"
    "11111111111111111111111111111111111111111111111111111111111111111111111"
    "1111111111111111111111111111111111111111111"
};


void test_expected_string_placed_in_buffer(void) {
    uint16_t expected[SEND_BUFFER_TEST];
    for (int tc = 0; tc < ARRAY_LENGTH(valid_lines); tc++) {
        usart1_trigger_transmission_Expect();
        memset(expected, 0, sizeof(expected));
        uart_put_raw_line(valid_lines[tc]);
        for (int indx = 0; indx < strlen(valid_lines[tc]); indx++) {
            expected[indx] = (uint16_t)valid_lines[tc][indx];
            usart1_send_Expect(expected[indx]);
            uart_send_response();
        }
        uint16_t *send_buf = get_send_buffer();
        TEST_ASSERT_EQUAL_UINT16_ARRAY(expected, send_buf, ARRAY_LENGTH(expected));
    }
}


int main(void) {
    UnityBegin("tests/test_serial.c");
    RUN_TEST(test_expected_string_placed_in_buffer);
    return UnityEnd();
}
