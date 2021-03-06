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

#include <category.h>

int mpm_category_free(void *tmp) {
    category_t  *ptr = tmp;

    if (ptr)
    {
        free(ptr->name);
        free(ptr->parent_name);
        if (ptr->parent != NULL)
        {
            ptr->parent->categ = NULL;
            mpm_package_free(ptr->parent);
            free(ptr->parent);
        }
    }
    return 1;
}

void mpm_category_init(category_t *ptr) {
    if (ptr)
    {
        ptr->name = NULL;
        ptr->parent_name = NULL;
        ptr->parent = NULL;
        ptr->id = 0;
    }
}
