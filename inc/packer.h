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

#ifndef PACKER_H
# define PACKER_H

# include <libmpm.h>
# include <sys/types.h>
# include <sys/stat.h>
# include <json.h>


# define PACKER_DEF_CONF_FN     "/package.json"

typedef struct packer_s {
    char        *dir;  /*!< Directory of the package */
    json_object *json; /*!< JSON object */
} packer_t;

/*!
 * \brief Allocate, fill and init a packer_t structure
 *
 * \param[in] dir Directory of the package
 *
 * \return A freshly allocated pointer on success, NULL on failure
 */
packer_t *packer_init(const char *dir);

/*!
 * \brief Free a packer_t structure
 *
 * \param[in,out] ptr Pointer to free
 */
void packer_free(packer_t *ptr);

/*!
 * \brief Read a directory and prepare the packer
 *
 * \param[in,out] ctx Already initialized packer_t struct
 *
 * \return true on success, false on failure
 */
bool packer_read(packer_t *ctx);

#endif /* PACKER_H */
