#include "test.h"

TEST(oui) {
	useless();
	TEST_ASSERT(true, "We'll, something's clearly wrong with your computer.");
	return TEST_SUCCESS;
}

int		main(void) {
	int		ret;

	reg_test("main", oui);
	ret = test_all();
	test_free();
	return ret;
}
