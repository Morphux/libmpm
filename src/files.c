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

#include <files.h>

/*!
 * \brief Initialize a file_t structure
 * \param ptr Structure to initialize
 */
void	mpm_file_init(file_t *ptr) {
	if (ptr) {
		ptr->path = NULL;
		ptr->parent = NULL;
		ptr->hash = NULL;
	}
}

/*!
 * \brief Free a file_t entry
 * \note Can be called as a callback of list_free
 */
int		mpm_file_free(void *tmp) {
	file_t *ptr = tmp;

	if (ptr) {
		free(ptr->path);
		free(ptr->hash);
		mpm_package_free(ptr->parent);
	}
	return 1;
}
