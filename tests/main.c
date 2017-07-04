#include "test.h"

int     main(void) {
    int         ret;

    mpm_init();

    register_test_error();
    register_flags_test();
    register_test_database();
    register_test_files();
    register_test_category();
    register_test_config();
    register_test_packer_file();
    register_test_compile();
    register_test_packer();

    m_info("Beginning tests...\n");
    ret = test_all();
    test_free();

    return ret;
}
