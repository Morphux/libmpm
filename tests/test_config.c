# include "test.h"

TEST(config_basic) {
	u8_t		ret;
	cfg_t		*conf = NULL;

	ret = parse_config("base.conf", &conf);
	TEST_ASSERT((ret == 0), "Can't open configuration file");
	cfg_free(conf);
	return TEST_SUCCESS;
}

void		register_test_config(void) {
	reg_test("config", config_basic);
}
