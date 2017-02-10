#include "test.h"

TEST(database_open_1) {
	database_t		*ptr = NULL;
	u8_t				ret = 0;

	ptr = mpm_database_open(&ret, NULL);
	TEST_ASSERT(ret == 0, "Database can't be opened");
	TEST_ASSERT((ptr != NULL), "Database can't be opened");
	ret = mpm_database_close(ptr);
	TEST_ASSERT(ret == 0, "Database can't be closed")
	return TEST_SUCCESS;
}

TEST(database_open_2) {
	database_t		*ptr = NULL;
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
	sql_result_t		*ptr = NULL;
	mlist_t				**head = context, *new = NULL;

	for (u8_t i = 0; i < col_num; i++) {
		ptr = malloc(sizeof(sql_result_t));
		assert(ptr != NULL);

		ptr->name = col_name[i] ? strdup(col_name[i]) : NULL;
		ptr->value = col_txt[i] ? strdup(col_txt[i]) : NULL;
		list_add(new, ptr, sizeof(sql_result_t));
		free(ptr);
	}
	list_add(*head, new, sizeof(mlist_t));
	free(new);
	return 0;
}

TEST(database_exec_1) {
	database_t		*ptr = NULL;
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
	database_t		*ptr = NULL;
	u8_t			ret = 0;
	char			*err = NULL;

	ptr = mpm_database_open(&ret, NULL);
	TEST_ASSERT((ret == 0), "Can't open the database");
	TEST_ASSERT((ptr != NULL), "Can't open the database");
	ret = mpm_database_exec(NULL, NULL, &exec_callback, ptr, &err);
	TEST_ASSERT((ret == 1), "NULL not handled correctly.");
	mpm_database_close(ptr);
	clean_db(DB_FN);
	return TEST_SUCCESS;
}

TEST(database_init_1) {
	database_t		*ptr = NULL;
	u8_t			ret = 0;

	ptr = mpm_database_open(&ret, NULL);
	TEST_ASSERT((ret == 0), "Can't open the database");
	TEST_ASSERT((ptr != NULL), "Can't open the database");
	ret = mpm_database_init(ptr);
	TEST_ASSERT((ret == 0), "Can't init the database");
	mpm_database_close(ptr);
	return TEST_SUCCESS;
}

TEST(database_init_2) {
	u8_t			ret = 0;

	ret = mpm_database_init(NULL);
	TEST_ASSERT((ret == 1), "Can't handle NULL pointer");
	return TEST_SUCCESS;
}

TEST(database_init_test_pkg_table) {
	database_t		*ptr = NULL;
	u8_t			ret = 0;
	mlist_t			*res = NULL, *tmp, *tmp2, *tmp3, *test = NULL;
	char			*err = NULL;
	sql_result_t	*result;

	ptr = mpm_database_open(&ret, NULL);
	TEST_ASSERT((ret == 0), "Can't open the database");
	TEST_ASSERT((ptr != NULL), "Can't open the database");
	ret = mpm_database_exec(ptr, "PRAGMA table_info([" PKG_TABLE "])",
		&exec_callback, &res, &err);
	TEST_ASSERT((ret == 0), "An error happened");
	TEST_ASSERT((err == NULL), "An error happened");
	mpm_database_close(ptr);

	list_for_each(res, tmp, tmp2) {
		list_for_each(tmp2, tmp3, result) {
			if (!strcmp(result->name, "name"))
				list_add(test, result->value, strlen(result->value));
		}
	}
	TEST_ASSERT((list_size(test) == 11), "Number of columns is wrong");
	list_free(test, NULL);
	free_sql_results(res);
	return TEST_SUCCESS;
}

TEST(database_init_test_files_table) {
	database_t		*ptr = NULL;
	u8_t			ret = 0;
	mlist_t			*res = NULL, *tmp, *tmp2, *tmp3, *test = NULL;
	char			*err = NULL;
	sql_result_t	*result;

	ptr = mpm_database_open(&ret, NULL);
	TEST_ASSERT((ret == 0), "Can't open the database");
	TEST_ASSERT((ptr != NULL), "Can't open the database");
	ret = mpm_database_exec(ptr, "PRAGMA table_info([" FILE_TABLE "])",
		&exec_callback, &res, &err);
	TEST_ASSERT((ret == 0), "An error happened");
	TEST_ASSERT((err == NULL), "An error happened");
	mpm_database_close(ptr);

	list_for_each(res, tmp, tmp2) {
		list_for_each(tmp2, tmp3, result) {
			if (!strcmp(result->name, "name"))
				list_add(test, result->value, strlen(result->value));
		}
	}
	TEST_ASSERT((list_size(test) == 6), "Number of columns is wrong");
	list_free(test, NULL);
	free_sql_results(res);
	return TEST_SUCCESS;
}

TEST(database_init_test_categ_table) {
	database_t		*ptr = NULL;
	u8_t			ret = 0;
	mlist_t			*res = NULL, *tmp, *tmp2, *tmp3, *test = NULL;
	char			*err = NULL;
	sql_result_t	*result;

	ptr = mpm_database_open(&ret, NULL);
	TEST_ASSERT((ret == 0), "Can't open the database");
	TEST_ASSERT((ptr != NULL), "Can't open the database");
	ret = mpm_database_exec(ptr, "PRAGMA table_info([" CAT_TABLE "])",
		&exec_callback, &res, &err);
	TEST_ASSERT((ret == 0), "An error happened");
	TEST_ASSERT((err == NULL), "An error happened");
	mpm_database_close(ptr);

	list_for_each(res, tmp, tmp2) {
		list_for_each(tmp2, tmp3, result) {
			if (!strcmp(result->name, "name"))
				list_add(test, result->value, strlen(result->value));
		}
	}
	TEST_ASSERT((list_size(test) == 4), "Number of columns is wrong");
	list_free(test, NULL);
	free_sql_results(res);
	return TEST_SUCCESS;
}



TEST(database_init_3) {
	database_t		*ptr = NULL;
	u8_t			ret = 0;
	int		st, fd[2];
	pid_t	pid;


	ptr = mpm_database_open(&ret, NULL);
	TEST_ASSERT((ret == 0), "Can't open the database");
	TEST_ASSERT((ptr != NULL), "Can't open the database");

	pipe(fd);
	if ((pid = fork()) == 0) {
		DUP_ALL_OUTPUTS(fd);
		ret = mpm_database_init(ptr);
		TEST_ASSERT((ret != 0), "Error did not trigger");
	} else {
		WAIT_AND_CLOSE(pid, st, fd);
	}
	mpm_database_close(ptr);
	clean_db(DB_FN);
	return TEST_SUCCESS;
}

TEST(database_add_pkg_1) {
	package_t	*pkg = NULL;
	category_t	*cat = NULL;
	database_t		*ptr = NULL;
	u8_t			ret = 0;

	ptr = mpm_database_open(&ret, NULL);
	TEST_ASSERT((ret == 0), "Can't open the database");
	TEST_ASSERT((ptr != NULL), "Can't open the database");

	ret = mpm_database_init(ptr);
	TEST_ASSERT((ret == 0), "Can't init database");

	pkg = malloc(sizeof(package_t));
	cat = malloc(sizeof(category_t));

	assert(pkg != NULL && cat != NULL);
	mpm_package_init(pkg);
	mpm_category_init(cat);
	pkg->name = strdup("test");
	pkg->version = strdup("1.0");
	cat->name = strdup("test2");
	cat->parent_name = strdup("test");
	pkg->categ = cat;
	pkg->desc = strdup("This a test package.");
	pkg->state = PACKAGE_STATE_USER_INSTALLED;
	pkg->deps = pkg->files = pkg->binaries = pkg->docs = pkg->config = NULL;
	ret = mpm_database_add_pkg(ptr, pkg);
	TEST_ASSERT((ret == 0), "Can't add a package in the database");
	mpm_database_close(ptr);
	mpm_package_free(pkg);
	free(pkg);
	return TEST_SUCCESS;
}

TEST(database_add_pkg_2) {
	u8_t	ret = 0;

	ret = mpm_database_add_pkg(NULL, NULL);
	TEST_ASSERT((ret == 1), "Can't handle NULL pointer");
	return TEST_SUCCESS;
}

TEST(database_add_file_1) {
	file_t		*file = NULL;
	package_t	*parent = NULL;
	database_t	*ptr = NULL;
	u8_t		ret = 0;

	(void)parent;
	ptr = mpm_database_open(&ret, NULL);
	TEST_ASSERT((ret == 0), "Can't open the database");
	TEST_ASSERT((ptr != NULL), "Can't open the database");

	ret = mpm_database_init(ptr);
	TEST_ASSERT((ret == 0), "Can't init database");

	file = malloc(sizeof(file_t));
	parent = malloc(sizeof(package_t));

	assert(file != NULL && parent != NULL);
	mpm_file_init(file);
	mpm_package_init(parent);
	file->path = strdup("/tmp/test");
	file->hash = strdup("Totally not a hash.");
	file->parent_name = strdup("Some Star Wars joke");
	file->type = FILE_TYPE_BIN;
	parent->id = 1;
	file->parent = parent;

	ret = mpm_database_add_file(ptr, file);
	TEST_ASSERT((ret == 0), "Can't add a file to the database");
	mpm_database_close(ptr);
	mpm_file_free(file);
	free(file);
	return TEST_SUCCESS;
}

TEST(database_add_file_2) {
	u8_t	ret = 0;

	ret = mpm_database_add_file(NULL, NULL);
	TEST_ASSERT((ret == 1), "Can't handle NULL pointer");
	return TEST_SUCCESS;
}

TEST(database_add_category_1) {
	category_t		*cat = NULL;
	package_t		*parent;
	database_t		*ptr = NULL;
	u8_t			ret = 0;

	ptr = mpm_database_open(&ret, NULL);
	TEST_ASSERT((ret == 0), "Can't open the database");
	TEST_ASSERT((ptr != NULL), "Can't open the database");

	ret = mpm_database_init(ptr);
	TEST_ASSERT((ret == 0), "Can't init the database");

	cat = malloc(sizeof(category_t));
	parent = malloc(sizeof(package_t));
	assert(cat != NULL);
	assert(parent != NULL);

	mpm_category_init(cat);
	mpm_package_init(parent);
	cat->name = strdup("test");
	parent->name = strdup("test_package");
	parent->id = 666;
	cat->parent = parent;
	parent->categ = cat;

	ret = mpm_database_add_categ(ptr, cat);
	TEST_ASSERT((ret == 0), "Cant' add the category");
	mpm_database_close(ptr);
	mpm_category_free(cat);
	free(cat);
	return TEST_SUCCESS;
}

TEST(database_add_category_2) {
	u8_t	ret = 0;

	ret = mpm_database_add_categ(NULL, NULL);
	TEST_ASSERT((ret == 1), "Can't handle NULL pointer");
	return TEST_SUCCESS;
}

TEST(database_get_pkg_by_id_1) {
	mlist_t			*lst;
	database_t		*ptr = NULL;
	u8_t			ret = 0;

	ptr = mpm_database_open(&ret, NULL);
	TEST_ASSERT((ret == 0), "Can't open the database");
	TEST_ASSERT((ptr != NULL), "Can't open the database");

	ret = mpm_get_package_by_id(ptr, 1, &lst);
	TEST_ASSERT((list_size(lst) == 1), "Can't find the package");
	mpm_database_close(ptr);
	list_free(lst, &mpm_package_free);
	return TEST_SUCCESS;
}

TEST(database_get_pkg_by_id_2) {
	mlist_t			*lst;
	u8_t			ret = 0;

	ret = mpm_get_package_by_id(NULL, 1, &lst);
	TEST_ASSERT((ret == 1), "Can't handle NULL pointer");
	return TEST_SUCCESS;
}

TEST(database_get_category_by_name_1) {
	mlist_t		*lst;
	database_t	*ptr = NULL;
	u8_t		ret = 0;

	ptr = mpm_database_open(&ret, NULL);
	TEST_ASSERT((ret == 0), "Can't open the database");
	TEST_ASSERT((ptr != NULL), "Can't open the database");

	ret = mpm_get_categ_by_name(ptr, "test", &lst);
	TEST_ASSERT((list_size(lst) == 1), "Can't find the category");
	mpm_database_close(ptr);
	list_free(lst, &mpm_category_free);
	return TEST_SUCCESS;
}

TEST(database_get_category_by_name_2) {
	mlist_t		*lst;
	u8_t		ret;

	ret = mpm_get_categ_by_name(NULL, NULL, &lst);
	TEST_ASSERT((ret == 1), "Can't handle NULL pointer");
	return TEST_SUCCESS;
}

TEST(database_get_category_by_name_3) {
	mlist_t		*lst;
	database_t	*ptr = NULL;
	u8_t		ret = 0;

	ptr = mpm_database_open(&ret, NULL);
	TEST_ASSERT((ret == 0), "Can't open the database");
	TEST_ASSERT((ptr != NULL), "Can't open the database");

	ret = mpm_get_categ_by_name(ptr, "non sense", &lst);
	TEST_ASSERT((list_size(lst) == 0), "Find the category !?");
	mpm_database_close(ptr);
	return TEST_SUCCESS;
}


TEST(database_get_pkg_by_name_1) {
	mlist_t			*lst;
	database_t		*ptr = NULL;
	u8_t			ret = 0;

	ptr = mpm_database_open(&ret, NULL);
	TEST_ASSERT((ret == 0), "Can't open the database");
	TEST_ASSERT((ptr != NULL), "Can't open the database");

	ret = mpm_get_package_by_name(ptr, "test", &lst);
	TEST_ASSERT((list_size(lst) == 1), "Can't find the package");
	mpm_database_close(ptr);
	list_free(lst, &mpm_package_free);
	return TEST_SUCCESS;
}

TEST(database_get_pkg_by_name_2) {
	mlist_t			*lst;
	u8_t			ret = 0;

	ret = mpm_get_package_by_name(NULL, "test", &lst);
	TEST_ASSERT((ret == 1), "Can't handle NULL pointer");
	clean_db(DB_FN);
	return TEST_SUCCESS;
}

TEST(database_sql_to_package) {
	package_t	*ptr = NULL;

	ptr = sql_to_package(NULL, NULL, NULL);
	TEST_ASSERT((ptr == NULL), "Pointer is not NULL");
	clean_db(DB_FN);
	return TEST_SUCCESS;
}

TEST(database_get_file_by_id_1) {
	mlist_t			*lst;
	database_t		*ptr = NULL;
	u8_t			ret = 0;

	ptr = mpm_database_open(&ret, NULL);
	TEST_ASSERT((ret == 0), "Can't open the database");
	TEST_ASSERT((ptr != NULL), "Can't open the database");

	ret = mpm_get_file_by_id(ptr, 1, &lst);
	TEST_ASSERT((list_size(lst) == 1), "Can't find the package");
	mpm_database_close(ptr);
	list_free(lst, &mpm_file_free);
	return TEST_SUCCESS;
}

TEST(database_get_file_by_id_2) {
	mlist_t			*lst;
	u8_t			ret = 0;

	ret = mpm_get_file_by_id(NULL, 1, &lst);
	TEST_ASSERT((ret == 1), "Can't handle NULL pointer");

	return TEST_SUCCESS;
}

TEST(database_get_file_by_id_3) {
	mlist_t			*lst;
	database_t		*ptr = NULL;
	u8_t			ret = 0;

	ptr = mpm_database_open(&ret, NULL);
	TEST_ASSERT((ret == 0), "Can't open the database");
	TEST_ASSERT((ptr != NULL), "Can't open the database");

	ret = mpm_get_file_by_id(ptr, 999, &lst);
	TEST_ASSERT((list_size(lst) == 0), "Found the package ?!");
	mpm_database_close(ptr);
	return TEST_SUCCESS;
}

TEST(database_get_file_by_parent_id_1) {
	mlist_t			*lst;
	database_t		*ptr = NULL;
	u8_t			ret = 0;

	ptr = mpm_database_open(&ret, NULL);
	TEST_ASSERT((ret == 0), "Can't open the database");
	TEST_ASSERT((ptr != NULL), "Can't open the database");

	ret = mpm_get_file_by_parent_id(ptr, 1, &lst);
	TEST_ASSERT((list_size(lst) == 1), "Can't find the package");
	mpm_database_close(ptr);
	list_free(lst, &mpm_file_free);
	return TEST_SUCCESS;
}

TEST(database_get_file_by_parent_id_2) {
	mlist_t			*lst;
	u8_t			ret = 0;

	ret = mpm_get_file_by_parent_id(NULL, 1, &lst);
	TEST_ASSERT((ret == 1), "Can't handle NULL pointer");

	return TEST_SUCCESS;
}

TEST(database_get_file_by_parent_id_3) {
	mlist_t			*lst;
	database_t		*ptr = NULL;
	u8_t			ret = 0;

	ptr = mpm_database_open(&ret, NULL);
	TEST_ASSERT((ret == 0), "Can't open the database");
	TEST_ASSERT((ptr != NULL), "Can't open the database");

	ret = mpm_get_file_by_parent_id(ptr, 999, &lst);
	TEST_ASSERT((list_size(lst) == 0), "Found the package ?!");
	mpm_database_close(ptr);
	return TEST_SUCCESS;
}


TEST(database_get_file_by_path_1) {
	mlist_t			*lst;
	database_t		*ptr = NULL;
	u8_t			ret = 0;

	ptr = mpm_database_open(&ret, NULL);
	TEST_ASSERT((ret == 0), "Can't open the database");
	TEST_ASSERT((ptr != NULL), "Can't open the database");

	ret = mpm_get_file_by_path(ptr, "/tmp/test", &lst);
	TEST_ASSERT((list_size(lst) == 1), "Can't find the package");
	mpm_database_close(ptr);
	list_free(lst, &mpm_file_free);
	return TEST_SUCCESS;
}

TEST(database_get_file_by_path_2) {
	mlist_t			*lst;
	u8_t			ret = 0;

	ret = mpm_get_file_by_path(NULL, NULL, &lst);
	TEST_ASSERT((ret == 1), "Can't handle NULL pointer");

	return TEST_SUCCESS;
}

TEST(database_get_file_by_path_3) {
	mlist_t			*lst;
	database_t		*ptr = NULL;
	u8_t			ret = 0;

	ptr = mpm_database_open(&ret, NULL);
	TEST_ASSERT((ret == 0), "Can't open the database");
	TEST_ASSERT((ptr != NULL), "Can't open the database");

	ret = mpm_get_file_by_path(ptr, "/non/sense", &lst);
	TEST_ASSERT((list_size(lst) == 0), "Found the package ?!");
	mpm_database_close(ptr);
	return TEST_SUCCESS;
}

TEST(database_get_file_by_parent_name_1) {
	mlist_t			*lst;
	database_t		*ptr = NULL;
	u8_t			ret = 0;

	ptr = mpm_database_open(&ret, NULL);
	TEST_ASSERT((ret == 0), "Can't open the database");
	TEST_ASSERT((ptr != NULL), "Can't open the database");

	ret = mpm_get_file_by_parent_name(ptr, "Some Star Wars joke", &lst);
	TEST_ASSERT((list_size(lst) == 1), "Can't find the package");
	mpm_database_close(ptr);
	list_free(lst, &mpm_file_free);
	return TEST_SUCCESS;
}

TEST(database_get_file_by_parent_name_2) {
	mlist_t			*lst;
	u8_t			ret = 0;

	ret = mpm_get_file_by_parent_name(NULL, NULL, &lst);
	TEST_ASSERT((ret == 1), "Can't handle NULL pointer");

	return TEST_SUCCESS;
}

TEST(database_get_file_by_parent_name_3) {
	mlist_t			*lst;
	database_t		*ptr = NULL;
	u8_t			ret = 0;

	ptr = mpm_database_open(&ret, NULL);
	TEST_ASSERT((ret == 0), "Can't open the database");
	TEST_ASSERT((ptr != NULL), "Can't open the database");

	ret = mpm_get_file_by_parent_name(ptr, "non_sense", &lst);
	TEST_ASSERT((list_size(lst) == 0), "Found the package ?!");
	mpm_database_close(ptr);
	return TEST_SUCCESS;
}


TEST(database_get_category_by_id_1) {
	mlist_t		*lst;
	database_t	*ptr = NULL;
	u8_t		ret = 0;

	ptr = mpm_database_open(&ret, NULL);
	TEST_ASSERT((ret == 0), "Can't open the database");
	TEST_ASSERT((ptr != NULL), "Can't open the database");

	ret = mpm_get_categ_by_id(ptr, 1, &lst);
	TEST_ASSERT((list_size(lst) == 1), "Can't find the package");
	mpm_database_close(ptr);
	list_free(lst, &mpm_category_free);
	return TEST_SUCCESS;
}

TEST(database_get_category_by_id_2) {
	mlist_t			*lst;
	u8_t			ret = 0;

	ret = mpm_get_categ_by_id(NULL, 1, &lst);
	TEST_ASSERT((ret == 1), "Can't handle NULL pointer");
	return TEST_SUCCESS;
}

TEST(database_sql_to_file) {
	file_t	*ptr = NULL;
	int		st, fd[2];
	pid_t	pid;

	ptr = sql_to_file(NULL, NULL, NULL);
	TEST_ASSERT((ptr == NULL), "Pointer is not NULL");

	ptr = malloc(sizeof(file_t));

	pipe(fd);
	if ((pid = fork()) == 0) {
		DUP_ALL_OUTPUTS(fd);
		ptr = sql_to_file(ptr, "Unknown", "Nothing");
	} else {
		WAIT_AND_CLOSE(pid, st, fd);
		TEST_ASSERT((WEXITSTATUS(st) == 1), "Exit code is wrong");
	}
	free(ptr);
	clean_db(DB_FN);
	return TEST_SUCCESS;
}

TEST(database_sql_to_categ) {
	category_t	*ptr = NULL;
	int		st, fd[2];
	pid_t	pid;

	ptr = sql_to_category(NULL, NULL, NULL);
	TEST_ASSERT((ptr == NULL), "Pointer is not NULL");

	ptr = malloc(sizeof(category_t));

	pipe(fd);
	if ((pid = fork()) == 0) {
		DUP_ALL_OUTPUTS(fd);
		sql_to_category(ptr, "Unknown", "Nothing");
                exit(0);
	} else {
		WAIT_AND_CLOSE(pid, st, fd);
		TEST_ASSERT((WEXITSTATUS(st) == 1), "Exit code is wrong");
	}
	free(ptr);
	clean_db(DB_FN);
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

int		free_sql_results(void *ptr) {
	mlist_t		*tmp = ptr, *it, *z, *j;

	if (tmp) {
		while (tmp != NULL) {
			it = tmp->next;
			z = tmp->member;
			while (z != NULL) {
				j = z->next;
				free_single_result_sql(z->member);
				free(z->member);
				free(z);
				z = j;
			}
			free(tmp);
			tmp = it;
		}
	}
	return 1;
}

int		free_single_result_sql(void *ptr) {
	sql_result_t	*tmp = ptr;

	if (tmp) {
		free(tmp->name);
		free(tmp->value);
		tmp->name = NULL;
		tmp->value = NULL;
	}
	return 1;
}

void		register_test_database(void) {
	reg_test("database", database_open_1);
	reg_test("database", database_open_2);
	reg_test("database", database_close_1);
	reg_test("database", database_exec_1);
	reg_test("database", database_exec_2);
	reg_test("database", database_init_1);
	reg_test("database", database_init_2);
	reg_test("database", database_init_test_pkg_table);
	reg_test("database", database_init_test_files_table);
	reg_test("database", database_init_test_categ_table);
	reg_test("database", database_init_3);
	reg_test("database", database_add_pkg_1);
	reg_test("database", database_add_pkg_2);
	reg_test("database", database_get_pkg_by_id_1);
	reg_test("database", database_get_pkg_by_id_2);
	reg_test("database", database_get_pkg_by_name_1);
	reg_test("database", database_get_pkg_by_name_2);
	reg_test("database", database_sql_to_package);
	reg_test("database", database_add_file_1);
	reg_test("database", database_add_file_2);
	reg_test("database", database_get_file_by_id_1);
	reg_test("database", database_get_file_by_id_2);
	reg_test("database", database_get_file_by_id_3);
	reg_test("database", database_get_file_by_path_1);
	reg_test("database", database_get_file_by_path_2);
	reg_test("database", database_get_file_by_path_3);
	reg_test("database", database_get_file_by_parent_id_1);
	reg_test("database", database_get_file_by_parent_id_2);
	reg_test("database", database_get_file_by_parent_id_3);
	reg_test("database", database_get_file_by_parent_name_1);
	reg_test("database", database_get_file_by_parent_name_2);
	reg_test("database", database_get_file_by_parent_name_3);
	reg_test("database", database_sql_to_file);
	reg_test("database", database_add_category_1);
	reg_test("database", database_add_category_2);
	reg_test("database", database_get_category_by_id_1);
	reg_test("database", database_get_category_by_id_2);
	reg_test("database", database_get_category_by_name_1);
	reg_test("database", database_get_category_by_name_2);
	reg_test("database", database_get_category_by_name_3);
	reg_test("database", database_sql_to_categ);
}
