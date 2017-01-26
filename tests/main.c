#include "test.h"

int		main(void) {
	int		ret;

	register_flags_test();
	register_test_database();
	ret = test_all();
	test_free();
	return ret;
}
