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

#ifndef CONFIG_H
# define CONFIG_H

# define _GNU_SOURCE         /* See feature_test_macros(7) */
# include <stdio.h>
# include <confuse.h>
# include <morphux.h>
# include <fcntl.h>
# include <error.h>


# define CONFIG_DEF_FN                  "mpm.conf"
# define CONFIG_DEF_PATH                "/etc/mpm/" CONFIG_DEF_FN

/* Main section */
# define CONFIG_MAIN_SEC                "main"
# define CONFIG_MAIN_INST_MODE          "install-mode"
# define CONFIG_MAIN_INST_MODE_DEF      "sources"
# define CONFIG_MAIN_THREAD_INST        "thread-install"
# define CONFIG_MAIN_THREAD_INST_DEF    0

/* Log section */
# define CONFIG_LOG_SEC                 "log"
# define CONFIG_LOG_VERBOSE_LVL         "verbose-level"
# define CONFIG_LOG_VERBOSE_LVL_DEF     1
# define CONFIG_LOG_DIR                 "directory"
# define CONFIG_LOG_DIR_DEF             "/var/log/mpm"

/* Download section */
# define CONFIG_DL_SEC                  "download"
# define CONFIG_DL_MAIN_SITE            "main-site"
# define CONFIG_DL_MAIN_SITE_DEF        "pkgs.morphux.org"
# define CONFIG_DL_THREAD_DL            "thread-downloading"
# define CONFIG_DL_THREAD_DL_DEF        0
# define CONFIG_DL_DIR                  "directory"
# define CONFIG_DL_DIR_DEF              "/var/cache/mpm/pkgs"
# define CONFIG_DL_MIRROR_LIST          "mirror-list"
# define CONFIG_DL_MIRROR_LIST_DEF      "/var/lib/mpm/mirrors"

/* Compile section */
# define CONFIG_COMP_SEC                "compile"
# define CONFIG_COMP_SBU                "sbu"
# define CONFIG_COMP_SBU_DEF            0
# define CONFIG_COMP_CONF_ARG           "configure-args"
# define CONFIG_COMP_CONF_ARG_DEF       "{\"--prefix=/usr\"}"
# define CONFIG_COMP_MAKE_ARG           "make-args"
# define CONFIG_COMP_MAKE_ARG_DEF       "{-j2}"

/* Packages section */
# define CONFIG_PKG_SEC                 "packages"
# define CONFIG_PKG_DB                  "database"
# define CONFIG_PKG_DB_DEF              "/var/lib/mpm/package.db"
# define CONFIG_PKG_TREE                "tree"
# define CONFIG_PKG_TREE_DEF            "/var/lib/mpm/tree"

/* Kernel section */
# define CONFIG_KERN_SEC                "kernel"
# define CONFIG_KERN_SRC                "sources"
# define CONFIG_KERN_SRC_DEF            "/usr/src/linux-current/"

/* Boot section */
# define CONFIG_BOOT_SEC                "boot"
# define CONFIG_BOOT_MNT_WHEN_NEEDED    "mount-when-needed"
# define CONFIG_BOOT_MNT_WHEN_NEEDED_DEF 0
# define CONFIG_BOOT_GRUB_DIR           "grub-dir"
# define CONFIG_BOOT_GRUB_DIR_DEF       "/boot/grub"

typedef struct config_s {
    cfg_t       *ptr;
    char        *fn;
    char        *err;
} config_t;

/*!
 * \brief Initialize a new config_t structure and read a config file
 * \param[in] path Path of the config file. If null, the default PATH is used
 * \param[out] ret Return code of the parse function
 *
 * This function will open a file, read a configuration in it.
 * If anything is wrong, the ret variable will be set to the proper error value.
 *
 * \return A freshly allocated structure
 */
config_t *parse_config(const char *path, u8_t *ret);

/*!
 * \brief Free a config_t pointer
 * \param[in,out] ptr Pointer to free
 */
void config_free(config_t **ptr);

/*!
 * \brief Get the parsing error string
 * \param[in] ptr Config context
 */
void config_get_error_string(config_t *ptr);

/*!
 * \brief Get a string of the configuration by its name
 *
 * \param ptr Configuration context
 * \param str Name of the member (Ex: "download.something")
 *
 * \return The value on success, NULL on failure
 */
char *get_conf_str_from_name(config_t *ptr, const char *str);

#endif /* CONFIG_H */
