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

u8_t		parse_config(const char *path, cfg_t **config) {
	cfg_opt_t	opts_main[] = {
		CFG_INT(CONFIG_MAIN_THREAD_INST,0,							CFGF_NONE),
		CFG_STR(CONFIG_MAIN_INST_MODE,	"binary",					CFGF_NONE),
		CFG_END()
	};
	cfg_opt_t	opts_log[] = {
		CFG_INT(CONFIG_LOG_VERBOSE_LVL,	1,							CFGF_NONE),
		CFG_STR(CONFIG_LOG_DIR,			"/var/log/mpm",				CFGF_NONE),
		CFG_END()
	};
	cfg_opt_t	opts_dl[] = {
		CFG_STR(CONFIG_DL_MAIN_SITE,	"pkgs.morphux.org",			CFGF_NONE),
		CFG_INT(CONFIG_DL_THREAD_DL,	0,							CFGF_NONE),
		CFG_STR(CONFIG_DL_DIR,			"/var/cache/mpm/pkgs/",		CFGF_NONE),
		CFG_STR(CONFIG_DL_MIRROR_LIST,	"/var/lib/mpm/mirrors",		CFGF_NONE),
		CFG_END()
	};
	cfg_opt_t	opts_comp[] = {
		CFG_INT(CONFIG_COMP_SBU,			0,						CFGF_NONE),
		CFG_STR_LIST(CONFIG_COMP_CONF_DEF,"{\"--prefix=/usr\"}",	CFGF_NONE),
		CFG_STR_LIST(CONFIG_COMP_MAKE_DEF,"{-j2}",					CFGF_NONE),
		CFG_END()
	};
	cfg_opt_t	opts_pkg[] = {
		CFG_STR(CONFIG_PKG_DB,			"/var/lib/mpm/package.db",	CFGF_NONE),
		CFG_STR(CONFIG_PKG_TREE,		"/var/cache/mpm/tree",		CFGF_NONE),
		CFG_END()
	};
	cfg_opt_t	opts_kern[] = {
		CFG_STR(CONFIG_KERN_SRC,		"/usr/src/linux-current",	CFGF_NONE),
		CFG_END()
	};
	cfg_opt_t	opts_boot[] = {
		CFG_INT(CONFIG_BOOT_MNT_WHEN_NEEDED, 0,						CFGF_NONE),
		CFG_STR(CONFIG_BOOT_GRUB_DIR,	"/boot/grub",				CFGF_NONE),
		CFG_END()
	};
	cfg_opt_t	opts[] = {
		CFG_SEC(CONFIG_MAIN_SEC,	opts_main,		CFGF_NONE),
		CFG_SEC(CONFIG_LOG_SEC,		opts_log,		CFGF_NONE),
		CFG_SEC(CONFIG_DL_SEC,		opts_dl,		CFGF_NONE),
		CFG_SEC(CONFIG_COMP_SEC,	opts_comp,		CFGF_NONE),
		CFG_SEC(CONFIG_PKG_SEC,		opts_pkg,		CFGF_NONE),
		CFG_SEC(CONFIG_KERN_SEC,	opts_kern,		CFGF_NONE),
		CFG_SEC(CONFIG_BOOT_SEC,	opts_boot,		CFGF_NONE),
		CFG_END()
	};
	u8_t		ret;

	*config = cfg_init(opts, CFGF_NONE);
	ret = cfg_parse(*config, path != NULL ? path : CONFIG_DEF_PATH);
	return ret;
}
