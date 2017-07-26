# include "test.h"

TEST(config_basic) {
    u8_t        ret;
    config_t    *conf = NULL;

    conf = parse_config("conf/base.conf", &ret);
    if (ret != 0) {
        m_panic("%s\n", conf->err);
    }
    TEST_ASSERT((ret == 0), "Can't open configuration file");
    config_free(&conf);
    return TEST_SUCCESS;
}

TEST(config_error) {
    u8_t                ret;
    config_t            *conf = NULL;

    conf = parse_config("conf/bad.conf", &ret);
    TEST_ASSERT((ret != 0), "No parsing error ?");
    TEST_ASSERT((strcmp(conf->err, "conf/bad.conf:1: no such option 'nonsense'") == 0), "Error is bad.");
    config_free(&conf);
    return TEST_SUCCESS;
}

TEST(config_get_error_string) {
    config_get_error_string(NULL);
    return TEST_SUCCESS;
}

# define CONF_FN "conf/base.conf"
static config_t *g_conf_test = NULL;

TEST(config_get_init) {
    u8_t        ret = 0;

    g_conf_test = parse_config(CONF_FN, &ret);
    TEST_ASSERT(ret == 0, "Could not parse configuration '" CONF_FN "'");
    return TEST_SUCCESS;
}

TEST(config_get_opt_from_name) {

    /* NULL pointers test*/
    TEST_ASSERT(get_opt_from_name(NULL, NULL) == NULL, "Return value is wrong");

    /* Strdup fail test */
    set_strdup_fail(0);
    TEST_ASSERT(get_opt_from_name(g_conf_test, "No care !") == NULL, "Strdup failure did not return the right value (Expected NULL)");

    /* No dot test */
    TEST_ASSERT(get_opt_from_name(g_conf_test, "No dot") == NULL, "Expected a NULL return value");

    /* Unknown section test */
    TEST_ASSERT(get_opt_from_name(g_conf_test, "Not_a_section.something") == NULL, "Expected a NULL return value");

    TEST_ASSERT(get_opt_from_name(g_conf_test, "compile.sbu") != NULL, "Expected a valid option");

    return TEST_SUCCESS;
}

TEST(config_get_conf_str_from_name) {
    TEST_ASSERT(get_conf_str_from_name(g_conf_test, "compiwefwefwefwefw") == NULL, "Expected a NULL value");

    TEST_ASSERT(get_conf_str_from_name(g_conf_test, "compile.make-args") != NULL, "Expected a non-NULL value");
    return TEST_SUCCESS;
}

TEST(config_get_conf_int_from_name) {
    int         ret = 0;

    TEST_ASSERT(get_conf_int_from_name(g_conf_test, "compiwefwefwefwefw", &ret) == false, "Expected a false value");

    TEST_ASSERT(get_conf_int_from_name(g_conf_test, "compile.sbu", &ret) != false, "Expected a non-false value");
    return TEST_SUCCESS;
}

TEST(config_get_type_from_name) {
    TEST_ASSERT(get_type_from_name(g_conf_test, "fwfwefwe") == CFGT_NONE, "Expected a NONE value on unknown token");

    TEST_ASSERT(get_type_from_name(g_conf_test, "compile.sbu") == CFGT_INT, "Expected a integer type for compile.sbu");
    return TEST_SUCCESS;
}

TEST(config_get_cleanup) {
    config_free(&g_conf_test);
    return TEST_SUCCESS;
}

void register_test_config(void) {
    reg_test("config", config_basic);
    reg_test("config", config_error);
    reg_test("config", config_get_error_string);
    reg_test("config", config_get_init);
    reg_test("config", config_get_opt_from_name);
    reg_test("config", config_get_conf_str_from_name);
    reg_test("config", config_get_conf_int_from_name);
    reg_test("config", config_get_type_from_name);
    reg_test("config", config_get_cleanup);
}
