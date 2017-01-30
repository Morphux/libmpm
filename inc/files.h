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

#ifndef FILES_H
# define FILES_H

# include <morphux.h>
# include <package.h>

typedef struct s_package package_t;

enum {
	FILE_TYPE_CONFIG,
	FILE_TYPE_BIN,
	FILE_TYPE_LIB,
	FILE_TYPE_OTHER
};

typedef struct		s_file {
	char			*path;
	u8_t			type;
	package_t		*parent;
	char			*hash;
}					file_t;

#endif /* FILES_H */
