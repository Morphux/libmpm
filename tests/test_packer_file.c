#include "test.h"

TEST(read_files_from_dir) {
    return TEST_SUCCESS;
}

void register_test_packer_file(void) {
    reg_test("packer_file", read_files_from_dir);
}
