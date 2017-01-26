#include "test.h"

TEST(database_open_1) {
	mdatabase_t		*ptr = NULL;
	u8_t				ret = 0;

	ptr = mpm_database_open(&ret);
	TEST_ASSERT(ret == 0, "Database can't be open");
	TEST_ASSERT((ptr != NULL), "Database can't be open");
	ret = mpm_database_close(ptr);
	TEST_ASSERT(ret == 0, "Database can't be close")
	return TEST_SUCCESS;
}

void		register_test_database(void) {
	reg_test("database", database_open_1);
}
