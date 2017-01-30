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
	sqlite3_close(ptr->sql);
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
	u8_t		error = 1;

	if (ptr != NULL) {
		error = sqlite3_close(ptr->sql);
		free(ptr);
	}
	return error;
}

/*!
 * \brief Execute a SQL query on an already opened database
 * \param ptr Opened database
 * \param query SQL Query
 * \param cl Query callback
 * \param ct First parameter of the callback (Context)
 * \param err Pointer on a string, to be filled with a string error.
 *
 * This function will execute an SQL query on an already opened database.
 * If the ptr or query parameter is NULL, the function will not do anything
 * and return 1.
 * The callback is used to get the result of a query.
 * The callback is defined and used trough defines, for readibilty purposes.
 * see SQL_CALLBACK_PTR and SQL_CALLBACK_DEF for more information.
 * If an SQL error happens, the err pointer is filled with a text error.
 * This string is allocated by the sqlite library, and should be freed by the
 * caller.
 */
u8_t			mpm_database_exec(mdatabase_t *ptr, const char *query,
		SQL_CALLBACK_PTR(cl), void *ct, char **err) {
	if (ptr == NULL || query == NULL)
		return 1;

	return sqlite3_exec(ptr->sql, query, cl, ct, err);
}

/**
 * int name(void *context, int col_num, char **col_txt, char **col_name)
 */
SQL_CALLBACK_DEF(callback_package_id) {
	mlist_t		*head = context;
	package_t	*ptr;

	if (col_num <= 0)
		return 0;

	ptr = malloc(sizeof(package_t));
	assert(ptr != NULL);

	for (u8_t i = 0; i < col_num; i++)
		sql_to_package(ptr, col_name[i], col_txt[i]);

	list_add(head, ptr, sizeof(ptr));
	return 0;
}

/*!
 * \brief Get a package by his Id
 * \param ptr Opened Database connection
 * \param id ID to search for
 * \param pkg Pointer on a list, used to store the results
 * \return Error code
 *
 * This function will search in an already opened database a package with a
 * given id.
 * A sql QUERY is constructed in this function, with the following content:
 * SELECT * FROM pkgs WHERE id = %d, where %d is the given id
 * This function will call list_add to add results to the given list,
 * caller should properly free this list.
 *
 * \note This function will set pkg to NULL before filling it with the results.
 * You should not call this function with an existing package list.
 */
u8_t			mpm_get_package_by_id(mdatabase_t *ptr, u64_t id,
						mlist_t **pkg) {
	char	*query;
	u8_t	ret;

	if (ptr == NULL)
		return 1;

	*pkg = NULL;
	asprintf(&query, QUERY_GET_PACKAGE_BY_ID(id));
	ret = sqlite3_exec(ptr->sql, query, &callback_package_id, *pkg, NULL);
	free(query);
	return ret;
}

/*!
 * \brief Fill a package_t structure with a SQL result
 * \param ptr Pointer to package_t. Must be NULL.
 * \param name Name of the column
 * \param val Value of the column
 *
 * This function will transform an SQL result, given through an sqlite callback,
 * and fill a package_t structure with it.
 * All the types conversion needed (strint -> *) are done in this function.
 *
 * \note If a unknown column is passed to this function, a panic will be throwed.
 */
package_t		*sql_to_package(package_t *ptr, char *name, char *val) {
	if (ptr == NULL)
		return ptr;

	if (strcmp(name, PKG_COL_ID) == 0) {
		ptr->id = strtoull(val, (char **)NULL, 10);
	} else if (strcmp(name, PKG_COL_NAME) == 0 ||
			strcmp(name, PKG_COL_VERSION) == 0 ||
			strcmp(name, PKG_COL_DESC) == 0) {
		ptr->name = strdup(val);
	} else if (strcmp(name, PKG_COL_STATE) == 0) {
		ptr->id = val[0] - '0';
	} else {
		m_panic("Unknown column '%s' in get_package\n", name);
	}
	return ptr;
}
