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

#include <database.h>

/*!
 * \brief Open a connection to a database
 * \param ret Return code, if any error
 * \param fn Database path
 * \return A fresh mdatabase_t structure
 *
 * This function will create a new mdatabase_t struct, allocate it, and open
 * a new database connection. In case of any error, the return value will be
 * NULL, and the ret pointer set to an error code.
 * If a value is passed to the param fn, the library will use this value as a 
 * database file path.
 */
mdatabase_t		*mpm_database_open(u8_t *ret, const char *fn) {
	mdatabase_t		*ptr;
	u8_t			error = 0;

	ptr = malloc(sizeof(mdatabase_t));
	assert(ptr != NULL);
	if (fn != NULL)
		error = sqlite3_open(fn, &ptr->sql);
	else
		error = sqlite3_open(DB_FN, &ptr->sql);
	if (error != 0)
		goto error;
	return ptr;

error:
	free(ptr);
	*ret = error;
	return NULL;
}

/*!
 * \brief Close a connection to an existing database
 * \param ptr An open database
 * \return The error code
 *
 * This function will close a database connection and free the passed struct.
 * On any error, this function will return the error code.
 */
u8_t			mpm_database_close(mdatabase_t *ptr) {
	u8_t		error;

	if (ptr != NULL) {
		error = sqlite3_close(ptr->sql);
		if (error != 0)
			return error;
		free(ptr);
	}
	return 0;
}
