#include "test.h"

int		main(void) {
	int		ret;

	register_flags_test();
	ret = test_all();
	test_free();
	return ret;
}
