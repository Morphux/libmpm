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

#include <flags.h>

static u8_t     mpm_flags[MPM_LAST] = {0};

void    mpm_set_flag(flag_index_t flag, u8_t val) {
    if (flag < MPM_LAST)
        mpm_flags[flag] = val;
}

u8_t    mpm_get_flag(flag_index_t flag) {
    assert(flag < MPM_LAST);
    return mpm_flags[flag];
}
