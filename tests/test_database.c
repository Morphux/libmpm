#include "test.h"

TEST(database_open_1) {
	mdatabase_t		*ptr = NULL;
	u8_t				ret = 0;

	ptr = mpm_database_open(&ret, NULL);
	TEST_ASSERT(ret == 0, "Database can't be opened");
	TEST_ASSERT((ptr != NULL), "Database can't be opened");
	ret = mpm_database_close(ptr);
	TEST_ASSERT(ret == 0, "Database can't be closed")
	return TEST_SUCCESS;
}

TEST(database_open_2) {
	mdatabase_t		*ptr = NULL;
	u8_t				ret = 0;

	ptr = mpm_database_open(&ret, "/illogical/path");
	TEST_ASSERT((ret == 14), "Database has been opened ?!");
	TEST_ASSERT((ptr == NULL), "Database can't be opened");
	mpm_database_close(ptr);
	return TEST_SUCCESS;
}


TEST(database_close_1) {
	u8_t			ret = 0;

	ret = mpm_database_close(NULL);
	TEST_ASSERT(ret != 0, "Error did not trigger");
	return TEST_SUCCESS;
}

/**
 * int name(void *context, int col_num, char **col_txt, char **col_name)
 */
SQL_CALLBACK_DEF(exec_callback) {
	(void)context;
	fprintf(stdout, "\n\nCol_num: %d\n", col_num);
	if (col_num == 0)
		return 0;
	while (*col_txt) {
		fprintf(stdout, "> %s\n", *col_txt);
		col_txt++;
	}

	while (*col_name) {
		fprintf(stdout, "> %s\n", *col_name);
		col_name++;
	}

	return 0;
}

TEST(database_exec_1) {
	mdatabase_t		*ptr = NULL;
	u8_t			ret = 0;
	char			*err = NULL;

	ptr = mpm_database_open(&ret, NULL);
	TEST_ASSERT((ret == 0), "Can't open the database");
	TEST_ASSERT((ptr != NULL), "Can't open the database");
	ret = mpm_database_exec(ptr, ";", &exec_callback, ptr, &err);
	TEST_ASSERT((err == NULL), "An error occured.");
	mpm_database_close(ptr);
	return TEST_SUCCESS;
}

TEST(database_exec_2) {
	mdatabase_t		*ptr = NULL;
	u8_t			ret = 0;
	char			*err = NULL;

	ptr = mpm_database_open(&ret, NULL);
	TEST_ASSERT((ret == 0), "Can't open the database");
	TEST_ASSERT((ptr != NULL), "Can't open the database");
	ret = mpm_database_exec(NULL, NULL, &exec_callback, ptr, &err);
	TEST_ASSERT((ret == 1), "NULL not handled correctly.");
	mpm_database_close(ptr);
	clean_db("test.db");
	return TEST_SUCCESS;
}

void		clean_db(const char *name) {
	int		st, fd[2];
	pid_t	pid;

	pipe(fd);
	if ((pid = fork()) == 0) {
		DUP_ALL_OUTPUTS(fd);
		execl("/bin/rm", "/bin/rm", name, NULL);
	} else {
		WAIT_AND_CLOSE(pid, st, fd);
	}
}

void		register_test_database(void) {
	reg_test("database", database_open_1);
	reg_test("database", database_open_2);
	reg_test("database", database_close_1);
	reg_test("database", database_exec_1);
	reg_test("database", database_exec_2);
}
