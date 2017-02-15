# include "test.h"

TEST(config_basic) {
	u8_t		ret;
	config_t		*conf = NULL;

	conf = parse_config("conf/base.conf", &ret);
	if (ret != 0) {
		m_panic("%s\n", conf->err);
	}
	TEST_ASSERT((ret == 0), "Can't open configuration file");
	config_free(conf);
	return TEST_SUCCESS;
}

TEST(config_error) {
	u8_t		ret;
	config_t		*conf = NULL;

	conf = parse_config("conf/bad.conf", &ret);
	TEST_ASSERT((ret != 0), "No parsing error ?");
        TEST_ASSERT((strcmp(conf->err, "conf/bad.conf:1: no such option 'nonsense'") == 0), "Error is bad.");
	config_free(conf);
	return TEST_SUCCESS;

}

void		register_test_config(void) {
	reg_test("config", config_basic);
	reg_test("config", config_error);
}
