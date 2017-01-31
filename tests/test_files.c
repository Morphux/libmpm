#include "test.h"

TEST(files_init) {
	file_t		*ptr;

	ptr = malloc(sizeof(file_t));
	mpm_file_init(ptr);
	TEST_ASSERT((ptr->path == NULL), "Init is wrong");
	TEST_ASSERT((ptr->parent == NULL), "Init is wrong");
	TEST_ASSERT((ptr->hash == NULL), "Init is wrong");
	mpm_file_free(ptr);
	free(ptr);
	return TEST_SUCCESS;
}

void		register_test_files(void) {
	reg_test("files", files_init);
}
