#include "test.h"

#define PACKAGE_OUTPUT_FN "package_compile" PACKER_DEF_EXT

TEST(init_compile) {
    packer_t        *ptr;

    ptr = packer_init_dir("packer/compilation//");
    TEST_ASSERT(packer_read_dir(ptr) == true, "An error happened");
    TEST_ASSERT(packer_create_archive(ptr, PACKAGE_OUTPUT_FN) == true,
                    "An error happened");

    packer_free(ptr);
    return TEST_SUCCESS;
}

TEST(configure_package) {
    packer_t    *ctx = packer_init_archive(PACKAGE_OUTPUT_FN);
    compile_t   *ptr = NULL;

    packer_extract_archive(ctx, "/tmp");
    ptr = package_install_init(ctx);
    TEST_ASSERT(ptr != NULL, "An error happened");
    TEST_ASSERT(before_package(ptr) == true, "An error happened");
    TEST_ASSERT(patch_package(ptr) == true, "An error happened");
    TEST_ASSERT(configure_package(ptr) == true, "An error happened");
    TEST_ASSERT(make_package(ptr) == true, "An error happened");
    TEST_ASSERT(install_package(ptr) == true, "An error happened");
    TEST_ASSERT(after_package(ptr) == true, "An error happened");
    TEST_ASSERT(package_install_cleanup(ptr) == true, "An error happened");

    return TEST_SUCCESS;
}

TEST(compile_cleanup) {
    unlink(PACKAGE_OUTPUT_FN);
    return TEST_SUCCESS;
}

void register_test_compile(void) {
    reg_test("compile", init_compile)
    reg_test("compile", configure_package);
    reg_test("compile", compile_cleanup);
}
