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

#ifndef ERROR_H
# define ERROR_H

# include <morphux.h>

# define SET_ERR_STR(num, str) [num] = str

typedef enum u32_t {
    /* No Error */
    ERR_NO_ERROR = 0,
    /* Insufficient memory */
    ERR_MEMORY,
    /* Bad pointer */
    ERR_BAD_PTR,
    /* chdir(2) call failed */
    ERR_CHDIR_FAILED,
    /* A binary execution failed */
    ERR_EXEC_FAILED,
    /* An extract of an archive failed */
    ERR_ARCHIVE_EXTRACT,
    /* An installation failed */
    ERR_INSTALLATION_FAILED,
    /* Bad configuration */
    ERR_BAD_CONFIG,
    /* Always keep this one last */
    ERR_LAST
} mpm_error_t;

/*!
 * \brief Set an error
 *
 * \param err_num Error number
 */
void set_mpm_error(mpm_error_t err_num);
# define SET_ERR(num) set_mpm_error(num)

/*!
 * \brief Get the error number
 *
 * \return The error number
 */
mpm_error_t get_mpm_error(void);
# define MPM_ERR() get_mpm_error()

/*!
 * \brief Return a string from an error code
 *
 * \brief err_num Error code
 *
 * \return Error, in string format
 */
const char *mpm_strerror(mpm_error_t err_num);

#endif
