#include "test.h"

TEST(database_open_1) {
	mdatabase_t		*ptr = NULL;
	u8_t				ret = 0;

	ptr = mpm_database_open(&ret, NULL);
	TEST_ASSERT(ret == 0, "Database can't be open");
	TEST_ASSERT((ptr != NULL), "Database can't be open");
	ret = mpm_database_close(ptr);
	TEST_ASSERT(ret == 0, "Database can't be close")
	return TEST_SUCCESS;
}

TEST(database_open_2) {
	mdatabase_t		*ptr = NULL;
	u8_t				ret = 0;

	ptr = mpm_database_open(&ret, "/illogical/path");
	TEST_ASSERT((ret == 14), "Database has be open ?!");
	TEST_ASSERT((ptr == NULL), "Database can't be open");
	return TEST_SUCCESS;
}


TEST(database_close_1) {
	mdatabase_t		*ptr = NULL;
	u8_t			ret = 0;

	/* Fake mallocs */
	ptr = malloc(sizeof(mdatabase_t));
	ptr->sql = malloc(10);

	ret = mpm_database_close(ptr);
	TEST_ASSERT(ret != 0, "Error did not trigger");
	free(ptr->sql);
	free(ptr);
	return TEST_SUCCESS;
}

void		register_test_database(void) {
	reg_test("database", database_open_1);
	reg_test("database", database_open_2);
	reg_test("database", database_close_1);
}
