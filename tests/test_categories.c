#include "test.h"

TEST(category_init) {
	category_t		*ptr;

	ptr = malloc(sizeof(category_t));
	mpm_category_init(ptr);
	TEST_ASSERT((ptr->name == NULL), "Init is wrong");
	mpm_category_free(ptr);
	free(ptr);
	return TEST_SUCCESS;
}

void	register_test_category(void) {
	reg_test("category", category_init);
}
