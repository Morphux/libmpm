# include "test.h"

TEST(config_basic) {
	u8_t		ret;
	config_t		*conf = NULL;

	conf = parse_config("base.conf", &ret);
	if (ret != 0) {
		m_panic("%s\n", conf->err);
	}
	TEST_ASSERT((ret == 0), "Can't open configuration file");
	config_free(conf);
	return TEST_SUCCESS;
}

void		register_test_config(void) {
	reg_test("config", config_basic);
}
