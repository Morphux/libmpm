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

#ifndef CATEGORY_H
# define CATEGORY_H

# include <morphux.h>

typedef struct		s_category category_t;
# include <libmpm.h>


# define CAT_TABLE				"categ"
# define CAT_COL_ID				"id"
# define CAT_COL_NAME			"name"
# define CAT_COL_PARENT			"parent"
# define CAT_COL_PARENT_NAME	"parent_name"

typedef struct		s_category {
	u64_t			id;
	char			*name;
	char			*parent_name;
	package_t		*parent;
}					category_t;


int		mpm_category_free(void *tmp);
void	mpm_category_init(category_t *ptr);

#endif /* CATEGORY_H */
