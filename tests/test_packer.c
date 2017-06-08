#include "test.h"

TEST(packer_tests_init) {
    /* Cleaning tests directory */
    recursive_delete("/tmp/test-2.0");
    return TEST_SUCCESS;
}

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

TEST(packer_wrong_main_1) {
    packer_t    *ptr;

    ptr = packer_init_dir("packer/wrong_main_1/");
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

TEST(packer_right_1) {
    packer_t    *ptr;

    ptr = packer_init_dir("packer/right_1/");
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

TEST(packer_wrong_package_section_6) {
    packer_t    *ptr;

    ptr = packer_init_dir("packer/wrong_package_section_6/");
    TEST_ASSERT(packer_read_dir(ptr) == false, "Error not raised");
    packer_free(ptr);
    return TEST_SUCCESS;
}

TEST(packer_wrong_package_section_7) {
    packer_t    *ptr;

    ptr = packer_init_dir("packer/wrong_package_section_7/");
    TEST_ASSERT(packer_read_dir(ptr) == false, "Error not raised");
    packer_free(ptr);
    return TEST_SUCCESS;
}

TEST(packer_wrong_package_section_8) {
    packer_t    *ptr;

    ptr = packer_init_dir("packer/wrong_package_section_8/");
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

TEST(packer_wrong_comp_section_9) {
    packer_t    *ptr;

    ptr = packer_init_dir("packer/wrong_comp_section_9/");
    TEST_ASSERT(packer_read_dir(ptr) == false, "Error not raised");
    packer_free(ptr);
    return TEST_SUCCESS;
}

TEST(packer_wrong_comp_section_10) {
    packer_t    *ptr;

    ptr = packer_init_dir("packer/wrong_comp_section_10/");
    TEST_ASSERT(packer_read_dir(ptr) == false, "Error not raised");
    packer_free(ptr);
    return TEST_SUCCESS;
}

TEST(packer_wrong_comp_section_11) {
    packer_t    *ptr;

    ptr = packer_init_dir("packer/wrong_comp_section_11/");
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

MPX_STATIC packer_t *packer_init(const char *str);
TEST(packer_init_1) {
    set_malloc_fail(0);
    TEST_ASSERT(packer_init("Yup.") == NULL, "Error not raised");
    return TEST_SUCCESS;
}

TEST(packer_init_2) {
    set_strdup_fail(0);
    TEST_ASSERT(packer_init("Yup.") == NULL, "Error not raised");
    return TEST_SUCCESS;
}

MPX_STATIC packer_header_t *packer_header_init(void);
TEST(packer_header_init) {
    set_malloc_fail(0);
    TEST_ASSERT(packer_header_init() == NULL, "Error not raised");
    return TEST_SUCCESS;
}

#define PACKAGE_OUTPUT_FN "package" PACKER_DEF_EXT

TEST(packer_create_archive_1) {
    packer_t        *ptr;

    ptr = packer_init_dir("packer/right/");
    TEST_ASSERT(packer_read_dir(ptr) == true, "An error happened");
    TEST_ASSERT(packer_create_archive(ptr, PACKAGE_OUTPUT_FN) == true,
                    "An error happened");

    packer_free(ptr);
    return TEST_SUCCESS;
}

TEST(packer_create_archive_wrong_fn) {
    packer_t    *ptr;

    ptr = packer_init_dir("packer/right/");
    TEST_ASSERT(packer_read_dir(ptr) == true, "An error happened");
    TEST_ASSERT(packer_create_archive(ptr, "/non/sense/path") == false,
                    "Error did not raise");
    packer_free(ptr);
    return TEST_SUCCESS;
}

TEST(packer_create_archive_wrong_type) {
    packer_t    *ptr;

    ptr = packer_init_archive("test");
    TEST_ASSERT(packer_create_archive(ptr, "nocare") == false,
                    "Error did not raise")
    packer_free(ptr);
    return TEST_SUCCESS;
}

TEST(packer_create_archive_2) {
    packer_t    *ptr;

    ptr = packer_init_dir("packer/right/");
    TEST_ASSERT(packer_read_dir(ptr) == true, "An error happened");

    set_malloc_fail(2);
    TEST_ASSERT(packer_create_archive(ptr, "package_2.mpx") == false,
                    "Error did not raise")

    set_malloc_fail(3);
    TEST_ASSERT(packer_create_archive(ptr, "package_2.mpx") == false,
                    "Error did not raise")

   set_malloc_fail(8);
    TEST_ASSERT(packer_create_archive(ptr, "package_2.mpx") == false,
                    "Error did not raise")

    set_malloc_fail(15);
    TEST_ASSERT(packer_create_archive(ptr, "package_2.mpx") == false,
                    "Error did not raise")

    packer_free(ptr);
    unlink("tests/package_2.mpx");
    return TEST_SUCCESS;
}

TEST(packer_create_archive_3) {
    packer_t *ptr;

    ptr = packer_init_dir("packer/right_1/");
    packer_read_dir(ptr);
    TEST_ASSERT(packer_create_archive(ptr, "package_2.mpx") == true,
        "An error happened");
    packer_free(ptr);
    return TEST_SUCCESS;
}

MPX_STATIC bool write_packer_sources(FILE *fd, packer_t *ctx, const char *dir_name);
TEST(packer_write_packer_sources) {
    FILE        *fd = fopen("package_2.mpx", "w+");
    packer_t    *ptr;

    ptr = packer_init_dir("packer/right/");

    set_calloc_fail(0);
    TEST_ASSERT(write_packer_sources(fd, ptr, "srcs/") == false, "Error did not raise");
    packer_free(ptr);
    fclose(fd);
    unlink("package_2.mpx");
    return TEST_SUCCESS;
}

MPX_STATIC int read_package_header_dependencies(const char *file, packer_t *ctx);
TEST(packer_read_package_header_dependencies) {
    char file[10], *tmp;
    u32_t i = htonl(3);

    memcpy(file, &i, sizeof(i));
    tmp = strdup("Test");
    file[4] = tmp;
    set_strdup_fail(0);
    TEST_ASSERT(read_package_header_dependencies(file, NULL) == 0, "Wrong return");
    free(tmp);
    return TEST_SUCCESS;
}

MPX_STATIC int read_package_header_compilation(char *file, packer_t *ctx);
TEST(packer_read_package_header_compilation) {
    char        *file;
    char        tmp[] = "TEst:123\0Oui\0make\0test\0install\0Test:123";
    u32_t       i = htonl(2);

    file = malloc(sizeof(i) + sizeof(tmp));
    memcpy(file, &i, sizeof(i));
    memcpy(file + sizeof(i), tmp, sizeof(tmp));

    set_malloc_fail(0);
    TEST_ASSERT(read_package_header_compilation(file, NULL) == 0, "Wrong return");
    memcpy(file + sizeof(i), tmp, sizeof(tmp));

    set_malloc_fail(1);
    TEST_ASSERT(read_package_header_compilation(file, NULL) == 0, "Wrong return");
    memcpy(file + sizeof(i), tmp, sizeof(tmp));

    set_malloc_fail(3);
    TEST_ASSERT(read_package_header_compilation(file, NULL) == 0, "Wrong return");
    memcpy(file + sizeof(i), tmp, sizeof(tmp));

    set_strdup_fail(0);
    TEST_ASSERT(read_package_header_compilation(file, NULL) == 0, "Wrong return");
    memcpy(file + sizeof(i), tmp, sizeof(tmp));

    set_strdup_fail(1);
    TEST_ASSERT(read_package_header_compilation(file, NULL) == 0, "Wrong return");
    memcpy(file + sizeof(i), tmp, sizeof(tmp));

    set_strdup_fail(2);
    TEST_ASSERT(read_package_header_compilation(file, NULL) == 0, "Wrong return");
    memcpy(file + sizeof(i), tmp, sizeof(tmp));

    set_strdup_fail(3);
    TEST_ASSERT(read_package_header_compilation(file, NULL) == 0, "Wrong return");
    memcpy(file + sizeof(i), tmp, sizeof(tmp));

    set_strdup_fail(4);
    TEST_ASSERT(read_package_header_compilation(file, NULL) == 0, "Wrong return");
    memcpy(file + sizeof(i), tmp, sizeof(tmp));

    set_strdup_fail(5);
    TEST_ASSERT(read_package_header_compilation(file, NULL) == 0, "Wrong return");

    free(file);
    return TEST_SUCCESS;
}

MPX_STATIC int read_package_header_package(const char *file, packer_t *ctx);
TEST(packer_read_package_header_package) {
    char tmp[] = "Name\0Version\0Desc\0";

    set_malloc_fail(0);
    TEST_ASSERT(read_package_header_package(tmp, NULL) == 0, "Wrong return");
    set_strdup_fail(0);
    TEST_ASSERT(read_package_header_package(tmp, NULL) == 0, "Wrong return");
    set_strdup_fail(1);
    TEST_ASSERT(read_package_header_package(tmp, NULL) == 0, "Wrong return");
    set_strdup_fail(2);
    TEST_ASSERT(read_package_header_package(tmp, NULL) == 0, "Wrong return");
    set_strdup_fail(3);
    TEST_ASSERT(read_package_header_package(tmp, NULL) == 0, "Wrong return");

    return TEST_SUCCESS;
}
MPX_STATIC bool read_package_header(char *file_content, packer_t *ctx, int *s_ret);
TEST(packer_read_package_header) {
    packer_t *ctx = packer_init_archive("Test");
    char    *file = mpm_read_file_from_fn(PACKAGE_OUTPUT_FN);
    int     ret = 0;

    TEST_ASSERT(read_package_header(NULL, ctx, &ret) == false, "Wrong return");
    TEST_ASSERT(read_package_header("pasmpx", ctx, &ret) == false, "Wrong return");

    set_malloc_fail(0);
    TEST_ASSERT(read_package_header(file, ctx, &ret) == false, "Wrong return");

    set_malloc_fail(1);
    TEST_ASSERT(read_package_header(file, ctx, &ret) == false, "Wrong return");

    set_malloc_fail(2);
    TEST_ASSERT(read_package_header(file, ctx, &ret) == false, "Wrong return");

    set_malloc_fail(9);
    TEST_ASSERT(read_package_header(file, ctx, &ret) == false, "Wrong return");

    free(file);
    packer_free(ctx);

    return TEST_SUCCESS;
}

MPX_STATIC packer_file_t *packer_file_init(const char *file, const char *dir);
TEST(packer_file_init) {
    char    *file = "truc";
    char    *dir = "machin";

    set_calloc_fail(0);
    TEST_ASSERT(packer_file_init(file, dir) == NULL, "Error did not raise");
    set_malloc_fail(0);
    TEST_ASSERT(packer_file_init(file, dir) == NULL, "Error did not raise");
    set_strcpy_fail(0);
    TEST_ASSERT(packer_file_init(file, dir) == NULL, "Error did not raise");
    set_strcat_fail(0);
    TEST_ASSERT(packer_file_init(file, dir) == NULL, "Error did not raise");
    return TEST_SUCCESS;
}

TEST(packer_extract_archive_1) {
    packer_t    *ctx = packer_init_archive(PACKAGE_OUTPUT_FN);

    TEST_ASSERT(packer_extract_archive(ctx, "/tmp") == true, "Wrong return");
    packer_free(ctx);
    return TEST_SUCCESS;
}

TEST(packer_read_archive_header) {
    packer_t *ctx = NULL;

    ctx = packer_init_dir("test/");
    TEST_ASSERT(packer_read_archive_header(ctx) == false, "Error did not raise");
    packer_free(ctx);

    ctx = packer_init_archive(PACKAGE_OUTPUT_FN);
    set_malloc_fail(0);
    TEST_ASSERT(packer_read_archive_header(ctx) == false, "Error did not raise");

    TEST_ASSERT(packer_read_archive_header(ctx) != false, "Something happend");
    TEST_ASSERT(strcmp(ctx->header->package.name, "test") == 0, "Name is wrong");
    TEST_ASSERT(strcmp(ctx->header->package.version, "2.0") == 0, "Name is wrong");

    packer_free(ctx);
    return TEST_SUCCESS;
}

TEST(packer_extract_archive_2) {
    packer_t    *ctx = NULL;

    ctx = packer_init_dir("Test");
    TEST_ASSERT(packer_extract_archive(ctx, NULL) == false, "Error did not raise");
    packer_free(ctx);

    ctx = packer_init_archive("Not a valid file");
    TEST_ASSERT(packer_extract_archive(ctx, NULL) == false, "Error did not raise");
    packer_free(ctx);

    recursive_delete("/tmp/test-2.0");
    set_mkdir_fail(-1);
    ctx = packer_init_archive(PACKAGE_OUTPUT_FN);
    set_strdup_fail(20);
    TEST_ASSERT(packer_extract_archive(ctx, "/tmp") == false, "Error did not raise");
    packer_free(ctx);

    ctx = packer_init_archive(PACKAGE_OUTPUT_FN);
    set_malloc_fail(0);
    TEST_ASSERT(packer_extract_archive(ctx, NULL) == false, "Error did not raise");
    packer_free(ctx);

    ctx = packer_init_archive(PACKAGE_OUTPUT_FN);
    TEST_ASSERT(packer_extract_archive(ctx, "/totally/not/valid/dir") == false, "Error did not raise");
    packer_free(ctx);

    ctx = packer_init_archive(PACKAGE_OUTPUT_FN);
    TEST_ASSERT(packer_extract_archive(ctx, "/") == false, "Error did not raise");
    packer_free(ctx);

    return TEST_SUCCESS;
}

TEST(packer_file_from_binary_to_disk) {
    off_t       ctr = 0;

    set_strdup_fail(0);
    TEST_ASSERT(packer_file_from_binary_to_disk("Content", &ctr) == false, "Error did not raise");

    set_strdup_fail(1);
    TEST_ASSERT(packer_file_from_binary_to_disk("/tmp/somedir/somefile", &ctr) == false, "Error did not raise");

    return TEST_SUCCESS;
}

TEST(get_file_information) {
    packer_file_t   *file;

    file = malloc(sizeof(*file));
    TEST_ASSERT(file != NULL, "Malloc failed");
    file->fn = strdup(PACKAGE_OUTPUT_FN);

    set_stat_fail(0);
    TEST_ASSERT(get_file_information(file) == false, "Error did not raise");
    free(file->fn);
    free(file);

    return TEST_SUCCESS;
}

TEST(packer_create_archive_cleanup) {
    recursive_delete("/tmp/test-2.0");
    unlink(PACKAGE_OUTPUT_FN);
    return TEST_SUCCESS;
}

void register_test_packer(void) {
    reg_test("packer", packer_tests_init);
    reg_test("packer", packer_init_dir);
    reg_test("packer", packer_init_archive);
    reg_test("packer", packer_read_dir_wrong_type);
    reg_test("packer", packer_read_dir_wrong_dir);
    reg_test("packer", packer_read_dir_no_configuration);
    reg_test("packer", packer_wrong_main_1);
    reg_test("packer", packer_right);
    reg_test("packer", packer_right_1);
    reg_test("packer", packer_wrong_package_section_1);
    reg_test("packer", packer_wrong_package_section_2);
    reg_test("packer", packer_wrong_package_section_3);
    reg_test("packer", packer_wrong_package_section_4);
    reg_test("packer", packer_wrong_package_section_5);
    reg_test("packer", packer_wrong_package_section_6);
    reg_test("packer", packer_wrong_package_section_7);
    reg_test("packer", packer_wrong_package_section_8);
    reg_test("packer", packer_wrong_comp_section_1);
    reg_test("packer", packer_wrong_comp_section_2);
    reg_test("packer", packer_wrong_comp_section_3);
    reg_test("packer", packer_wrong_comp_section_4);
    reg_test("packer", packer_wrong_comp_section_5);
    reg_test("packer", packer_wrong_comp_section_6);
    reg_test("packer", packer_wrong_comp_section_7);
    reg_test("packer", packer_wrong_comp_section_8);
    reg_test("packer", packer_wrong_comp_section_9);
    reg_test("packer", packer_wrong_comp_section_10);
    reg_test("packer", packer_wrong_comp_section_11);
    reg_test("packer", packer_wrong_deps_section_1);
    reg_test("packer", packer_wrong_deps_section_2);
    reg_test("packer", packer_init_1);
    reg_test("packer", packer_init_2);
    reg_test("packer", packer_header_init);
    reg_test("packer", packer_create_archive_1);
    reg_test("packer", packer_create_archive_2);
    reg_test("packer", packer_create_archive_3);
    reg_test("packer", packer_write_packer_sources);
    reg_test("packer", packer_create_archive_wrong_fn);
    reg_test("packer", packer_create_archive_wrong_type);
    reg_test("packer", packer_read_package_header_dependencies);
    reg_test("packer", packer_read_package_header_compilation);
    reg_test("packer", packer_read_package_header_package);
    reg_test("packer", packer_read_package_header);
    reg_test("packer", packer_file_init);
    reg_test("packer", packer_extract_archive_1);
    reg_test("packer", packer_extract_archive_2);
    reg_test("packer", packer_read_archive_header);
    reg_test("packer", packer_file_from_binary_to_disk);
    reg_test("packer", get_file_information);
    reg_test("packer", packer_create_archive_cleanup);
}
