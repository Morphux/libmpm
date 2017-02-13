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

# include <confuse.h>
# include <morphux.h>

# define CONFIG_DEF_PATH "/etc/mpm/mpm.conf"

# define CONFIG_MAIN_SEC "main"
# define CONFIG_MAIN_INST_MODE "install-mode"
# define CONFIG_MAIN_THREAD_INST "thread-install"

# define CONFIG_LOG_SEC "log"
# define CONFIG_LOG_VERBOSE_LVL "verbose-level"
# define CONFIG_LOG_DIR "directory"

# define CONFIG_DL_SEC "download"
# define CONFIG_DL_MAIN_SITE "main-site"
# define CONFIG_DL_THREAD_DL "thread-downloading"
# define CONFIG_DL_DIR "directory"
# define CONFIG_DL_MIRROR_LIST "mirror-list"

# define CONFIG_COMP_SEC "compile"
# define CONFIG_COMP_SBU "sbu"
# define CONFIG_COMP_CONF_DEF "configure-def"
# define CONFIG_COMP_MAKE_DEF "make-def"

# define CONFIG_PKG_SEC "packages"
# define CONFIG_PKG_DB "database"
# define CONFIG_PKG_TREE "tree"

# define CONFIG_KERN_SEC "kernel"
# define CONFIG_KERN_SRC "sources"

# define CONFIG_BOOT_SEC "boot"
# define CONFIG_BOOT_MNT_WHEN_NEEDED "mount-when-needed"
# define CONFIG_BOOT_GRUB_DIR "grub-dir"

u8_t		parse_config(const char *path, cfg_t **config);

#endif /* CONFIG_H */
