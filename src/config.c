/*********************************** LICENSE **********************************\
*                            Copyright 2017 Morphux                            *
*                                                                              *
*        Licensed under the Apache License, Version 2.0 (the "License");       *
*        you may not use this file except in compliance with the License.      *
*                  You may obtain a copy of the License at                     *
*                                                                              *
*                 http://www.apache.org/licenses/LICENSE-2.0                   *
*                                                                              *
*      Unless required by applicable law or agreed to in writing, software     *
*       distributed under the License is distributed on an "AS IS" BASIS,      *
*    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  *
*        See the License for the specific language governing permissions and   *
*                       limitations under the License.                         *
\******************************************************************************/

#include "config.h"

static char g_error[ERROR_MAX_LEN] = "";

static void config_error_cb(cfg_t *ptr, const char *fmt, va_list ap) {
    char        err[250];

    (void)ptr;
    vsnprintf(err, 250, fmt, ap);

    /* Prevent buffer overflow */
    if (strlen(err) < 250)
        strcpy(g_error, err);
}

config_t *parse_config(const char *path, u8_t *ret) {
    config_t    *config = NULL;

    /* Main section */
    cfg_opt_t   opts_main[] = {
        CFG_INT(CONFIG_MAIN_THREAD_INST,CONFIG_MAIN_THREAD_INST_DEF, CFGF_NONE),
        CFG_STR(CONFIG_MAIN_INST_MODE,  CONFIG_MAIN_INST_MODE_DEF,   CFGF_NONE),
        CFG_END()
    };

    /* Log section */
    cfg_opt_t   opts_log[] = {
        CFG_INT(CONFIG_LOG_VERBOSE_LVL, CONFIG_LOG_VERBOSE_LVL_DEF,  CFGF_NONE),
        CFG_STR(CONFIG_LOG_DIR,         CONFIG_LOG_DIR_DEF,          CFGF_NONE),
        CFG_END()
    };

    /* Download section */
    cfg_opt_t   opts_dl[] = {
        CFG_STR(CONFIG_DL_MAIN_SITE,    CONFIG_DL_MAIN_SITE_DEF,     CFGF_NONE),
        CFG_INT(CONFIG_DL_THREAD_DL,    CONFIG_DL_THREAD_DL_DEF,     CFGF_NONE),
        CFG_STR(CONFIG_DL_DIR,          CONFIG_DL_DIR_DEF,           CFGF_NONE),
        CFG_STR(CONFIG_DL_MIRROR_LIST,  CONFIG_DL_MIRROR_LIST_DEF,   CFGF_NONE),
        CFG_END()
    };

    /* Compile section */
    cfg_opt_t   opts_comp[] = {
        CFG_INT(CONFIG_COMP_SBU,           CONFIG_COMP_SBU_DEF,      CFGF_NONE),
        CFG_STR_LIST(CONFIG_COMP_CONF_ARG, CONFIG_COMP_CONF_ARG_DEF, CFGF_NONE),
        CFG_STR_LIST(CONFIG_COMP_MAKE_ARG, CONFIG_COMP_MAKE_ARG_DEF, CFGF_NONE),
        CFG_END()
    };

    /* Package section */
    cfg_opt_t   opts_pkg[] = {
        CFG_STR(CONFIG_PKG_DB,         CONFIG_PKG_DB_DEF,            CFGF_NONE),
        CFG_STR(CONFIG_PKG_TREE,       CONFIG_PKG_TREE_DEF,          CFGF_NONE),
        CFG_END()
    };

    /* Kernel section */
    cfg_opt_t   opts_kern[] = {
        CFG_STR(CONFIG_KERN_SRC,       CONFIG_KERN_SRC_DEF,          CFGF_NONE),
        CFG_END()
    };

        /* Boot section */
    cfg_opt_t   opts_boot[] = {
        CFG_INT(CONFIG_BOOT_MNT_WHEN_NEEDED, CONFIG_BOOT_MNT_WHEN_NEEDED_DEF, CFGF_NONE),
        CFG_STR(CONFIG_BOOT_GRUB_DIR,   CONFIG_BOOT_GRUB_DIR_DEF,    CFGF_NONE),
        CFG_END()
    };

        /* All sections */
    cfg_opt_t   opts[] = {
        CFG_SEC(CONFIG_MAIN_SEC,        opts_main,      CFGF_NONE),
        CFG_SEC(CONFIG_LOG_SEC,         opts_log,       CFGF_NONE),
        CFG_SEC(CONFIG_DL_SEC,          opts_dl,        CFGF_NONE),
        CFG_SEC(CONFIG_COMP_SEC,        opts_comp,      CFGF_NONE),
        CFG_SEC(CONFIG_PKG_SEC,         opts_pkg,       CFGF_NONE),
        CFG_SEC(CONFIG_KERN_SEC,        opts_kern,      CFGF_NONE),
        CFG_SEC(CONFIG_BOOT_SEC,        opts_boot,      CFGF_NONE),
        CFG_END()
    };

    assert(ret != NULL);
    config = malloc(sizeof(config_t));
    assert(config != NULL);

    /* Structure initialization */
    config->fn = NULL;
    config->err = NULL;
    config->need_save = false;
    config->ptr = cfg_init(opts, CFGF_NONE);
    cfg_set_error_function(config->ptr, &config_error_cb);

    /* Parse the config file */
    *ret = cfg_parse(config->ptr, path != NULL ? path : CONFIG_DEF_PATH);

    config->fn = malloc(strlen(path != NULL ? path : CONFIG_DEF_PATH) + 1);
    strcpy(config->fn, path != NULL ? path : CONFIG_DEF_PATH);

    if (*ret != 0)
    {
        set_mpm_error(ERR_BAD_CONFIG);
        config_get_error_string(config);
    }

    return config;
}

void config_free(config_t **ptr) {
    if (ptr != NULL && *ptr != NULL)
    {
        cfg_free((*ptr)->ptr);
        free((*ptr)->fn);
        free((*ptr)->err);
        free(*ptr);
        *ptr = NULL;
    }
}

void config_get_error_string(config_t *ptr) {
    if (strlen(g_error) == 0 || ptr == NULL)
        return ;

    asprintf(&ptr->err, "%s:%d: %s", ptr->fn, ptr->ptr->line, g_error);
    strcpy(g_error, "");
}

cfg_opt_t *get_opt_from_name(config_t *ptr, const char *str)
{
    cfg_t       *sec;
    cfg_opt_t   *opt = NULL;
    char        *section, *member, *dup;
    size_t      i;

    if (str == NULL || ptr == NULL)
        return NULL;

    dup = strdup(str);
    if (dup == NULL)
        return NULL;

    for (i = 0; dup[i] != '\0' && dup[i] != '.'; i++)
        ;

    if (dup[i] == '\0')
    {
        free(dup);
        return NULL;
    }

    dup[i] = '\0';
    section = dup;
    member = dup + i + 1;

    sec = cfg_getsec(ptr->ptr, section);
    if (sec == NULL)
    {
        free(dup);
        return NULL;
    }

    opt = cfg_getopt(sec, member);
    free(dup);
    return opt;
}

cfg_type_t get_type_from_name(config_t *ptr, const char *str) {
    cfg_opt_t   *opt = NULL;

    opt = get_opt_from_name(ptr, str);
    return opt->type;
}

char *get_conf_str_from_name(config_t *ptr, const char *str) {
    cfg_opt_t   *opt = NULL;

    opt = get_opt_from_name(ptr, str);
    if (opt == NULL)
        return NULL;

    /* Cannot not be a string */
    assert(opt->type == CFGT_STR);
    return cfg_opt_getnstr(opt, 0);
}

bool get_conf_int_from_name(config_t *ptr, const char *str, int *ret) {
    cfg_opt_t   *opt = NULL;

    opt = get_opt_from_name(ptr, str);
    if (opt == NULL)
        return false;

    /* Cannot not be a int */
    assert(opt->type == CFGT_INT);
    *ret = cfg_opt_getnint(opt, 0);
    return true;
}
