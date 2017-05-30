#include "test.h"

TEST(set_mpm_error) {
    TEST_ASSERT(get_mpm_error() == ERR_NO_ERROR, "Default value for errors is wrong");
    set_mpm_error(ERR_MEMORY);
    return TEST_SUCCESS;
}

TEST(get_mpm_error) {
    TEST_ASSERT(get_mpm_error() == ERR_MEMORY, "Value is wrong");
    TEST_ASSERT(strcmp(mpm_strerror(get_mpm_error()), "Insufficient memory") == 0,
        "String is wrong");
    TEST_ASSERT(get_mpm_error() == ERR_NO_ERROR, "Default value for errors is wrong");
    return TEST_SUCCESS;
}

void register_test_error(void) {
    reg_test("error", set_mpm_error);
    reg_test("error", get_mpm_error);
}
