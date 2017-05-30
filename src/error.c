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

#include <error.h>

static u32_t    g_error = ERR_NO_ERROR;
static const char *g_str_errors[] = {
    SET_ERR_STR(ERR_NO_ERROR, "No error"),
    SET_ERR_STR(ERR_MEMORY, "Insufficient memory"),
    SET_ERR_STR(ERR_BAD_PTR, "Bad pointer"),
    SET_ERR_STR(ERR_CHDIR_FAILED, "Can't change directory"),
    SET_ERR_STR(ERR_OPEN, "Can't open a file"),
    SET_ERR_STR(ERR_MKDIR_FAILED, "Can't create a directory"),
    SET_ERR_STR(ERR_EXEC_FAILED, "A binary execution failed"),
    SET_ERR_STR(ERR_ARCHIVE_EXTRACT, "An archive extraction failed"),
    SET_ERR_STR(ERR_BAD_ARCHIVE_TYPE, "Bad archive type"),
    SET_ERR_STR(ERR_NOT_A_PACKAGE, "File is not a Morphux package"),
    SET_ERR_STR(ERR_INSTALLATION_FAILED, "An installation failed"),
    SET_ERR_STR(ERR_BAD_CONFIG, "Bad configuration"),
    SET_ERR_STR(ERR_BAD_JSON, "JSON file wrongly formatted"),
    SET_ERR_STR(ERR_BAD_JSON_TYPE, "Unexecpted JSON type")
};

void set_mpm_error(mpm_error_t err_num) {
    g_error = err_num;
}

u32_t get_mpm_error(void) {
    return g_error;
}

const char *mpm_strerror(mpm_error_t err_num) {
    const char *ret = NULL;

    if (err_num > 0 && err_num < ERR_LAST)
        ret = g_str_errors[err_num];
    err_num = ERR_NO_ERROR;
    return ret;
}
