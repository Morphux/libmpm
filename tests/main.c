#include "test.h"

int		main(void) {
	int		ret;

	register_flags_test();
	register_test_database();
	register_test_files();
	register_test_category();
	register_test_config();
	ret = test_all();
	test_free();
	return ret;
}
