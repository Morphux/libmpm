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

TEST(packer_wrong_package_section_1) {
    packer_t    *ptr;

    ptr = packer_init_dir("packer/wrong_package_section_1/");
    TEST_ASSERT(packer_read_dir(ptr) == false, "Error not raised");
    packer_free(ptr);
    return TEST_SUCCESS;
}

TEST(packer_wrong_package_section_2) {
    packer_t    *ptr;

    ptr = packer_init_dir("packer/wrong_package_section_2/");
    TEST_ASSERT(packer_read_dir(ptr) == false, "Error not raised");
    packer_free(ptr);
    return TEST_SUCCESS;
}

TEST(packer_wrong_package_section_3) {
    packer_t    *ptr;

    ptr = packer_init_dir("packer/wrong_package_section_3/");
    TEST_ASSERT(packer_read_dir(ptr) == false, "Error not raised");
    packer_free(ptr);
    return TEST_SUCCESS;
}

TEST(packer_wrong_package_section_4) {
    packer_t    *ptr;

    ptr = packer_init_dir("packer/wrong_package_section_4/");
    TEST_ASSERT(packer_read_dir(ptr) == false, "Error not raised");
    packer_free(ptr);
    return TEST_SUCCESS;
}

TEST(packer_wrong_package_section_5) {
    packer_t    *ptr;

    ptr = packer_init_dir("packer/wrong_package_section_5/");
    TEST_ASSERT(packer_read_dir(ptr) == false, "Error not raised");
    packer_free(ptr);
    return TEST_SUCCESS;
}

TEST(packer_wrong_comp_section_1) {
    packer_t    *ptr;

    ptr = packer_init_dir("packer/wrong_comp_section_1/");
    TEST_ASSERT(packer_read_dir(ptr) == false, "Error not raised");
    packer_free(ptr);
    return TEST_SUCCESS;
}

TEST(packer_wrong_comp_section_2) {
    packer_t    *ptr;

    ptr = packer_init_dir("packer/wrong_comp_section_2/");
    TEST_ASSERT(packer_read_dir(ptr) == false, "Error not raised");
    packer_free(ptr);
    return TEST_SUCCESS;
}

TEST(packer_wrong_comp_section_3) {
    packer_t    *ptr;

    ptr = packer_init_dir("packer/wrong_comp_section_3/");
    TEST_ASSERT(packer_read_dir(ptr) == false, "Error not raised");
    packer_free(ptr);
    return TEST_SUCCESS;
}

TEST(packer_wrong_comp_section_4) {
    packer_t    *ptr;

    ptr = packer_init_dir("packer/wrong_comp_section_4/");
    TEST_ASSERT(packer_read_dir(ptr) == false, "Error not raised");
    packer_free(ptr);
    return TEST_SUCCESS;
}

TEST(packer_wrong_comp_section_5) {
    packer_t    *ptr;

    ptr = packer_init_dir("packer/wrong_comp_section_5/");
    TEST_ASSERT(packer_read_dir(ptr) == false, "Error not raised");
    packer_free(ptr);
    return TEST_SUCCESS;
}

TEST(packer_wrong_comp_section_6) {
    packer_t    *ptr;

    ptr = packer_init_dir("packer/wrong_comp_section_6/");
    TEST_ASSERT(packer_read_dir(ptr) == false, "Error not raised");
    packer_free(ptr);
    return TEST_SUCCESS;
}

TEST(packer_wrong_comp_section_7) {
    packer_t    *ptr;

    ptr = packer_init_dir("packer/wrong_comp_section_7/");
    TEST_ASSERT(packer_read_dir(ptr) == false, "Error not raised");
    packer_free(ptr);
    return TEST_SUCCESS;
}

TEST(packer_wrong_comp_section_8) {
    packer_t    *ptr;

    ptr = packer_init_dir("packer/wrong_comp_section_8/");
    TEST_ASSERT(packer_read_dir(ptr) == false, "Error not raised");
    packer_free(ptr);
    return TEST_SUCCESS;
}

TEST(packer_wrong_deps_section_1) {
    packer_t    *ptr;

    ptr = packer_init_dir("packer/wrong_deps_section_1/");
    TEST_ASSERT(packer_read_dir(ptr) == false, "Error not raised");
    packer_free(ptr);
    return TEST_SUCCESS;
}

TEST(packer_wrong_deps_section_2) {
    packer_t    *ptr;

    ptr = packer_init_dir("packer/wrong_deps_section_2/");
    TEST_ASSERT(packer_read_dir(ptr) == false, "Error not raised");
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
    reg_test("packer", packer_wrong_package_section_1);
    reg_test("packer", packer_wrong_package_section_2);
    reg_test("packer", packer_wrong_package_section_3);
    reg_test("packer", packer_wrong_package_section_4);
    reg_test("packer", packer_wrong_package_section_5);
    reg_test("packer", packer_wrong_comp_section_1);
    reg_test("packer", packer_wrong_comp_section_2);
    reg_test("packer", packer_wrong_comp_section_3);
    reg_test("packer", packer_wrong_comp_section_4);
    reg_test("packer", packer_wrong_comp_section_5);
    reg_test("packer", packer_wrong_comp_section_6);
    reg_test("packer", packer_wrong_comp_section_7);
    reg_test("packer", packer_wrong_comp_section_8);
    reg_test("packer", packer_wrong_deps_section_1);
    reg_test("packer", packer_wrong_deps_section_2);
}
