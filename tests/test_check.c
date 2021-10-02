#include "unity.h"


void setUp(void) {
}


void tearDown(void) {
}


void test_build(void) {
    TEST_ASSERT_EQUAL(0, 0);
}


int main(void) {
    UnityBegin("tests/test_check.c");
    RUN_TEST(test_build);
    return UnityEnd();
}
