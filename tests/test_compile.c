#include "test.h"

#define PACKAGE_OUTPUT_FN "package_compile" PACKER_DEF_EXT

# define OUTPUT_DIR "/tmp/test-2.0/"

TEST(init_compile) {
    packer_t        *ptr;

    set_mkdir_fail(-1);
    recursive_delete(OUTPUT_DIR);
    ptr = packer_init_dir("packer/compilation/");
    TEST_ASSERT(packer_read_dir(ptr) == true, "An error happened");
    TEST_ASSERT(packer_create_archive(ptr, PACKAGE_OUTPUT_FN) == true,
                    "An error happened");

    packer_free(ptr);
    return TEST_SUCCESS;
}

TEST(before_package) {
    packer_t    *ctx = packer_init_archive(PACKAGE_OUTPUT_FN);
    compile_t   *ptr = NULL;

    TEST_ASSERT(packer_extract_archive(ctx, "/tmp") == true, "An error happened");
    ptr = package_install_init(ctx);
    TEST_ASSERT(ptr != NULL, "An error happened");
    set_fork_fail(0);
    TEST_ASSERT(before_package(ptr) == false, "Error did not raise");
    chdir("/");
    TEST_ASSERT(before_package(ptr) == true, "An error happened");
    TEST_ASSERT(package_install_cleanup(ptr) == true, "An error happened");
    recursive_delete(OUTPUT_DIR);

    return TEST_SUCCESS;
}

TEST(patch_package) {
    packer_t    *ctx = packer_init_archive(PACKAGE_OUTPUT_FN);
    compile_t   *ptr = NULL;

    TEST_ASSERT(packer_extract_archive(ctx, "/tmp") == true, "An error happened");
    ptr = package_install_init(ctx);
    TEST_ASSERT(ptr != NULL, "An error happened");

    set_chdir_fail(0);
    TEST_ASSERT(patch_package(ptr) == false, "Error did not raise");

    set_chdir_fail(1);
    TEST_ASSERT(patch_package(ptr) == false, "Error did not raise");

    chdir("/");
    TEST_ASSERT(patch_package(ptr) == true, "Error did not raise");

    TEST_ASSERT(package_install_cleanup(ptr) == true, "An error happened");
    recursive_delete(OUTPUT_DIR);

    return TEST_SUCCESS;
}

TEST(configure_package) {
    packer_t    *ctx = packer_init_archive(PACKAGE_OUTPUT_FN);
    compile_t   *ptr = NULL;
    mlist_t     *tmp;

    TEST_ASSERT(packer_extract_archive(ctx, "/tmp") == true, "An error happened");
    ptr = package_install_init(ctx);
    TEST_ASSERT(ptr != NULL, "An error happened");

    tmp = ptr->package->header->compilation->configure;
    ptr->package->header->compilation->configure = NULL;
    TEST_ASSERT(configure_package(ptr) == true, "Return is wrong");
    ptr->package->header->compilation->configure = tmp;

    set_chdir_fail(0);
    TEST_ASSERT(configure_package(ptr) == false, "Error did not raise");

    TEST_ASSERT(package_install_cleanup(ptr) == true, "An error happened");
    recursive_delete(OUTPUT_DIR);
    return TEST_SUCCESS;
}

TEST(make_package) {
    packer_t    *ctx = packer_init_archive(PACKAGE_OUTPUT_FN);
    compile_t   *ptr = NULL;
    char        *tmp;

    TEST_ASSERT(packer_extract_archive(ctx, "/tmp") == true, "An error happened");
    ptr = package_install_init(ctx);
    TEST_ASSERT(ptr != NULL, "An error happened");

    tmp = ptr->package->header->compilation->make;
    ptr->package->header->compilation->make = NULL;
    TEST_ASSERT(make_package(ptr) == true, "An error happened");
    ptr->package->header->compilation->make = tmp;

    set_chdir_fail(0);
    TEST_ASSERT(make_package(ptr) == false, "Error did not raise");

    set_fork_fail(0);
    TEST_ASSERT(make_package(ptr) == false, "Error did not raise");

    TEST_ASSERT(package_install_cleanup(ptr) == true, "An error happened");
    recursive_delete(OUTPUT_DIR);
    return TEST_SUCCESS;
}

TEST(install_package) {
    packer_t    *ctx = packer_init_archive(PACKAGE_OUTPUT_FN);
    compile_t   *ptr = NULL;
    char        *tmp;

    TEST_ASSERT(packer_extract_archive(ctx, "/tmp") == true, "An error happened");
    ptr = package_install_init(ctx);
    TEST_ASSERT(ptr != NULL, "An error happened");

    tmp = ptr->package->header->compilation->install;
    ptr->package->header->compilation->install = NULL;
    TEST_ASSERT(install_package(ptr) == true, "An error happened");
    ptr->package->header->compilation->install = tmp;

    set_chdir_fail(0);
    TEST_ASSERT(install_package(ptr) == false, "Error did not raise");

    set_fork_fail(0);
    TEST_ASSERT(install_package(ptr) == false, "Error did not raise");

    TEST_ASSERT(package_install_cleanup(ptr) == true, "An error happened");
    recursive_delete(OUTPUT_DIR);
    return TEST_SUCCESS;
}

TEST(after_package) {
    packer_t    *ctx = packer_init_archive(PACKAGE_OUTPUT_FN);
    compile_t   *ptr = NULL;

    TEST_ASSERT(packer_extract_archive(ctx, "/tmp") == true, "An error happened");
    ptr = package_install_init(ctx);
    TEST_ASSERT(ptr != NULL, "An error happened");
    set_fork_fail(0);
    TEST_ASSERT(after_package(ptr) == false, "Error did not raise");
    chdir("/");
    TEST_ASSERT(after_package(ptr) == true, "An error happened");
    TEST_ASSERT(package_install_cleanup(ptr) == true, "An error happened");
    recursive_delete(OUTPUT_DIR);

    return TEST_SUCCESS;
}

TEST(package_install_init) {
    packer_t    *ctx = packer_init_archive(PACKAGE_OUTPUT_FN);
    compile_t   *ptr = NULL;
    mlist_t     *tmp;

    TEST_ASSERT(packer_extract_archive(ctx, "/tmp") == true, "An error happened");
    set_malloc_fail(0);
    TEST_ASSERT(package_install_init(ctx) == NULL, "Error did not raise");

    set_chdir_fail(0);
    TEST_ASSERT(package_install_init(ctx) == NULL, "Error did not raise");

    tmp = ctx->header->compilation->env;
    ctx->header->compilation->env = NULL;

    ptr = package_install_init(ctx);
    TEST_ASSERT(ptr != NULL, "Error did not raise");
    ctx->header->compilation->env = tmp;

    TEST_ASSERT(package_install_cleanup(ptr) == true, "An error happened");
    recursive_delete(OUTPUT_DIR);

    return TEST_SUCCESS;

}

TEST(package_install_cleanup) {
    packer_t    *ctx = packer_init_archive(PACKAGE_OUTPUT_FN);
    compile_t   *ptr = NULL;

    TEST_ASSERT(packer_extract_archive(ctx, "/tmp") == true, "An error happened");
    ptr = package_install_init(ctx);
    TEST_ASSERT(ptr != NULL, "An error happened");

    set_chdir_fail(0);
    TEST_ASSERT(package_install_cleanup(ptr) == false, "Error did not raise");
    TEST_ASSERT(package_install_cleanup(ptr) == true, "An error happened");
    recursive_delete(OUTPUT_DIR);

    return TEST_SUCCESS;
}

TEST(full_install) {
    packer_t    *ctx = packer_init_archive(PACKAGE_OUTPUT_FN);

    TEST_ASSERT(install_archive(ctx) == true, "An error happened")
    return TEST_SUCCESS;
}

TEST(full_install_2) {
    packer_t    *ctx;

    ctx = packer_init_archive(PACKAGE_OUTPUT_FN);

    set_malloc_fail(0);
    TEST_ASSERT(install_archive(ctx) == false, "Error did not raise");

    ctx = packer_init_archive(PACKAGE_OUTPUT_FN);
    set_malloc_fail(9);
    TEST_ASSERT(install_archive(ctx) == false, "Error did not raise");

    ctx = packer_init_archive(PACKAGE_OUTPUT_FN);
    set_fork_fail(0);
    TEST_ASSERT(install_archive(ctx) == false, "Error did not raise");

    ctx = packer_init_archive(PACKAGE_OUTPUT_FN);
    set_fork_fail(1);
    TEST_ASSERT(install_archive(ctx) == false, "Error did not raise");

    ctx = packer_init_archive(PACKAGE_OUTPUT_FN);
    set_fork_fail(2);
    TEST_ASSERT(install_archive(ctx) == false, "Error did not raise");

    ctx = packer_init_archive(PACKAGE_OUTPUT_FN);
    set_fork_fail(3);
    TEST_ASSERT(install_archive(ctx) == false, "Error did not raise");

    ctx = packer_init_archive(PACKAGE_OUTPUT_FN);
    set_fork_fail(4);
    TEST_ASSERT(install_archive(ctx) == false, "Error did not raise");

    ctx = packer_init_archive(PACKAGE_OUTPUT_FN);
    set_fork_fail(5);
    TEST_ASSERT(install_archive(ctx) == false, "Error did not raise");

    return TEST_SUCCESS;
}

TEST(compile_cleanup) {
    unlink(PACKAGE_OUTPUT_FN);
    return TEST_SUCCESS;
}

void register_test_compile(void) {
    reg_test("compile", init_compile)
    reg_test("compile", before_package);
    reg_test("compile", patch_package);
    reg_test("compile", configure_package);
    reg_test("compile", make_package);
    reg_test("compile", install_package);
    reg_test("compile", after_package);
    reg_test("compile", full_install);
    reg_test("compile", package_install_init);
    reg_test("compile", package_install_cleanup);
    reg_test("compile", full_install_2);
    reg_test("compile", compile_cleanup);
}
