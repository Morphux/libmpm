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

#ifndef PACKAGE_H
# define PACKAGE_H

# include <morphux.h>
# include <category.h>
# include <files.h>

enum {
	PACKAGE_STATE_USER_INSTALLED,
	PACKAGE_STATE_DEPENDENCY,
	PACKAGE_STATE_ORPHAN
};

# define PKG_TABLE			"pkgs"
# define PKG_COL_ID			"id"
# define PKG_COL_NAME		"name"
# define PKG_COL_VERSION	"version"
# define PKG_COL_CATEG		"categ"
# define PKG_COL_DESC		"desc"
# define PKG_COL_STATE		"state"
# define PKG_COL_DEPS		"deps"
# define PKG_COL_FILES		"files"
# define PKG_COL_BINARIES	"binaries"
# define PKG_COL_CONFIG		"config"
# define PKG_COL_DOCS		"docs"

typedef struct		s_package {
	u64_t			id;
	char			*name;
	char			*version;
	category_t		*categ;
	char			*desc;
	u8_t			state;
	mlist_t			*deps;
	mlist_t			*files;
	mlist_t			*binaries;
	mlist_t			*config;
	mlist_t			*docs;
}					package_t;

#endif /* PACKAGE_H */
