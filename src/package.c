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

# include <package.h>

/*!
 * \brief Free a package entry
 * \note Can be used as list_free callback
 */
int             mpm_package_free(void *tmp) {
    package_t   *ptr = tmp;

    if (ptr)
    {
        free(ptr->name);
        free(ptr->version);
        free(ptr->desc);
        if (ptr->categ != NULL)
        {
            ptr->categ->parent = NULL;
            mpm_category_free(ptr->categ);
            free(ptr->categ);
        }
        /*list_free(ptr->deps, &package_free);*/
        /* TODO: Files free */
    }
    return 1;
}

/*!
 * \brief Initialize a package entry
 * \param ptr Pointer to an allocated ptr
 */
void    mpm_package_init(package_t *ptr) {
    if (ptr)
    {
        ptr->id = 0;
        ptr->state = PACKAGE_STATE_ORPHAN;
        ptr->name = NULL;
        ptr->version = NULL;
        ptr->categ = NULL;
        ptr->desc = NULL;
        ptr->deps = NULL;
        ptr->files = NULL;
        ptr->binaries = NULL;
        ptr->config = NULL;
        ptr->docs = NULL;
    }
}
