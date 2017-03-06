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

# define FILE_TABLE             "files"
# define FILE_COL_ID            "id"
# define FILE_COL_PATH          "path"
# define FILE_COL_TYPE          "type"
# define FILE_COL_PARENT        "parent"
# define FILE_COL_PARENT_NAME   "parent_name"
# define FILE_COL_HASH          "hash"

typedef struct s_package package_t;

typedef enum    file_type_e {
    FILE_TYPE_CONFIG = 1,
    FILE_TYPE_BIN,
    FILE_TYPE_LIB,
    FILE_TYPE_OTHER
}               file_type_t;

typedef struct          file_s {
    u64_t       id;
    char        *path;
    file_type_t type;
    package_t   *parent;
    char        *parent_name;
    char        *hash;
}                       file_t;

/*!
 * \brief Initialize a file_t structure
 * \param[in,out] ptr Structure to initialize
 */
void    mpm_file_init(file_t *ptr);

/*!
 * \brief Free a file_t entry
 * \param[in] tmp Pointer to free
 * \note Can be called as a callback of list_free
 */
int     mpm_file_free(void *tmp);

# include <package.h>
#endif /* FILES_H */
