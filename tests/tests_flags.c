#include "test.h"

TEST(flag_set_base_1) {
	mpm_set_flag(MPM_VERBOSE_LEVEL, 2);
	TEST_ASSERT((mpm_get_flag(MPM_VERBOSE_LEVEL) == 2), "Value is wrong");
	return TEST_SUCCESS;
}

TEST(flag_set_base_2) {
	mpm_set_flag(150, 8);
	TEST_ASSERT((mpm_get_flag(MPM_VERBOSE_LEVEL) == 2), "Value is wrong");
	return TEST_SUCCESS;
}


void		register_flags_test(void) {
	reg_test("flags", flag_set_base_1);
	reg_test("flags", flag_set_base_2);
}
