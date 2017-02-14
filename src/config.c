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

/*!
 * \brief Initialize a new config_t structure and read a config file
 * \param path Path of the config file. If null, the default PATH is used
 * \param ret Return code of the parse function
 *
 * This function will open a file, read a configuration in it.
 * If anything is wrong, the ret variable will be set to the proper error value.
 *
 * \return A freshly allocated structure
 */
config_t        *parse_config(const char *path, u8_t *ret) {
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

    config = malloc(sizeof(config_t));
    assert(config != NULL);
    config->ptr = cfg_init(opts, CFGF_NONE);
    cfg_set_error_function(config->ptr, &config_error_cb);
    *ret = cfg_parse(config->ptr, path != NULL ? path : CONFIG_DEF_PATH);

    if (*ret != 0) {
        config_free(config);
        return NULL;
    }
    config->fn = malloc(strlen(path != NULL ? path : CONFIG_DEF_PATH));
    strcpy(config->fn, path != NULL ? path : CONFIG_DEF_PATH);
    return config;
}

/*!
 * \brief Free a config_t pointer
 * \param ptr Pointer to free
 */
void            config_free(config_t *ptr) {
    if (ptr != NULL) {
        cfg_free(ptr->ptr);
        free(ptr);
    }
}

void            config_error_cb(cfg_t *ptr, const char *fmt, va_list ap) {
    char        err[250];
    /* TODO: get error through call */
    vsprintf(err, fmt, ap);
    printf("\n%s - %d: %s\n", ptr->filename, ptr->line, err);
}
