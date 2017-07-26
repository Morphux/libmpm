#include "test.h"

TEST(read_files_from_dir) {
    mlist_t     *files = NULL, *dirs = NULL;

    set_stat_fail(0);
    TEST_ASSERT(read_files_from_dir("/tmp/", &files, &dirs) == false, "Error did not raise");
    return TEST_SUCCESS;
}

# define _TEST_FN "test_get_file_information_libmpm"
TEST(file_get_file_information) {
    FILE *fd = fopen("/tmp/" _TEST_FN, "w+");

    fprintf(fd, "This is the file content");
    fclose(fd);
    TEST_ASSERT(get_file_information(NULL) == false, "Error did not raise");

    packer_file_t *ptr = packer_file_init(_TEST_FN, "/tmp/");

    TEST_ASSERT(ptr != NULL, "Cannot allocate the packer_file_t ptr");
    /* Fail on chunk = malloc(...) */
    set_malloc_fail(1);
    TEST_ASSERT_FMT(get_file_information(ptr) == false, "Error did not raise %s", GET_ERR_STR());

    /* Fail on file->content = malloc(...) */
    set_malloc_fail(2);
    TEST_ASSERT(get_file_information(ptr) == false, "Error did not raise");

    set_malloc_fail(-1);
    packer_file_free(ptr);
    free(ptr);

    return TEST_SUCCESS;
}

void register_test_packer_file(void) {
    reg_test("packer_file", read_files_from_dir);
    reg_test("packer_file", file_get_file_information);
}
