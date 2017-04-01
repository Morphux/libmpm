#include "test.h"

TEST(packer_init_dir) {
    packer_t    *ptr = NULL;

    set_strdup_fail(1);
    set_malloc_fail(1);
    ptr = packer_init_dir("Test 123");
    TEST_ASSERT(ptr != NULL, "Function failed");
    TEST_ASSERT(strcmp(ptr->str, "Test 123") == 0, "Directory name is wrong");
    packer_free(ptr);
    ptr = packer_init_dir("Test 123");
    TEST_ASSERT(ptr == NULL, "Error not raised");
    ptr = packer_init_dir("Test 123");
    TEST_ASSERT(ptr == NULL, "Error not raised");
    return TEST_SUCCESS;
}

TEST(packer_init_archive) {
    packer_t    *ptr = NULL;

    set_strdup_fail(1);
    set_malloc_fail(1);
    ptr = packer_init_archive("Test 123");
    TEST_ASSERT(ptr != NULL, "Function failed");
    TEST_ASSERT(strcmp(ptr->str, "Test 123") == 0, "Directory name is wrong");
    packer_free(ptr);
    ptr = packer_init_archive("Test 123");
    TEST_ASSERT(ptr == NULL, "Error not raised");
    ptr = packer_init_archive("Test 123");
    TEST_ASSERT(ptr == NULL, "Error not raised");
    return TEST_SUCCESS;
}

TEST(packer_read_dir_wrong_type) {
    packer_t    *ptr;

    ptr = packer_init_archive("Test");
    TEST_ASSERT(packer_read_dir(ptr) == false, "Error not raised");
    packer_free(ptr);
    return TEST_SUCCESS;
}

TEST(packer_read_dir_wrong_dir) {
    packer_t    *ptr;

    ptr = packer_init_dir("Test/non/existing");
    TEST_ASSERT(packer_read_dir(ptr) == false, "Error not raised");
    packer_free(ptr);
    return TEST_SUCCESS;
}

TEST(packer_read_dir_no_configuration) {
    packer_t    *ptr;

    ptr = packer_init_dir("packer/wrong/");
    TEST_ASSERT(packer_read_dir(ptr) == false, "Error not raised");
    packer_free(ptr);
    return TEST_SUCCESS;
}

TEST(packer_right) {
    packer_t    *ptr;

    ptr = packer_init_dir("packer/right/");
    TEST_ASSERT(packer_read_dir(ptr) == true, "An error happened");
    packer_free(ptr);
    return TEST_SUCCESS;
}

void register_test_packer(void) {
    reg_test("packer", packer_init_dir);
    reg_test("packer", packer_init_archive);
    reg_test("packer", packer_read_dir_wrong_type);
    reg_test("packer", packer_read_dir_wrong_dir);
    reg_test("packer", packer_read_dir_no_configuration);
    reg_test("packer", packer_right);
}
