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

/**
 * Array of strings, used to translate errors in an human readable string
 */
static const char *g_str_errors[] = {
    __SET_ERR_STR(ERR_NO_ERROR, "No error"),
    __SET_ERR_STR(ERR_MEMORY, "Insufficient memory"),
    __SET_ERR_STR(ERR_BAD_PTR, "Bad pointer"),
    __SET_ERR_STR(ERR_CHDIR_FAILED, "Can't change directory"),
    __SET_ERR_STR(ERR_OPEN, "Can't open a file"),
    __SET_ERR_STR(ERR_MKDIR_FAILED, "Can't create a directory"),
    __SET_ERR_STR(ERR_EXEC_FAILED, "A binary execution failed"),
    __SET_ERR_STR(ERR_ARCHIVE_EXTRACT, "An archive extraction failed"),
    __SET_ERR_STR(ERR_BAD_ARCHIVE_TYPE, "Bad archive type"),
    __SET_ERR_STR(ERR_NOT_A_PACKAGE, "File is not a Morphux package"),
    __SET_ERR_STR(ERR_INSTALLATION_FAILED, "An installation failed"),
    __SET_ERR_STR(ERR_BAD_CONFIG, "Bad configuration"),
    __SET_ERR_STR(ERR_BAD_JSON, "JSON file wrongly formatted"),
    __SET_ERR_STR(ERR_BAD_JSON_TYPE, "Unexecpted JSON type")
};
static char g_str_error[ERROR_MAX_LEN] = "";

void set_mpm_error(mpm_error_t err_num) {
    g_error = err_num;
}

u32_t get_mpm_error(void) {
    return g_error;
}

const char *mpm_strerror(mpm_error_t err_num) {
    const char *ret = NULL;

    /* Check for pontentials overflow */
    if (err_num > 0 && err_num < ERR_LAST)
        ret = g_str_errors[err_num];

    g_error = ERR_NO_ERROR;
    return ret;
}

void mpm_set_str_error(const char *str, ...) {
    va_list     ap;

    va_start(ap, str);
    vsnprintf(g_str_error, ERROR_MAX_LEN, str, ap);
    va_end(ap);
}

char *mpm_get_str_error(void) {
    return g_str_error;
}
