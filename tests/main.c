#include "test.h"

int		main(void) {
	int		ret;

	register_flags_test();
	register_test_database();
	register_test_files();
	register_test_category();
	ret = test_all();
	test_free();
	return ret;
}
