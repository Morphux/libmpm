#include "test.h"

TEST(packer_init) {
    packer_t    *ptr = NULL;

    set_strdup_fail(1);
    set_malloc_fail(1);
    ptr = packer_init("Test 123");
    TEST_ASSERT(ptr != NULL, "Function failed");
    TEST_ASSERT(strcmp(ptr->dir, "Test 123") == 0, "Directory name is wrong");
    packer_free(ptr);
    ptr = packer_init("Test 123");
    TEST_ASSERT(ptr == NULL, "Error not raised");
    ptr = packer_init("Test 123");
    TEST_ASSERT(ptr == NULL, "Error not raised");
    return TEST_SUCCESS;
}

void register_test_packer(void) {
    reg_test("packer", packer_init);
}
