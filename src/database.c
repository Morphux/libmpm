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
 * \return A fresh database_t structure
 *
 * This function will create a new database_t struct, allocate it, and open
 * a new database connection. In case of any error, the return value will be
 * NULL, and the ret pointer set to an error code.
 * If a value is passed to the param fn, the library will use this value as a 
 * database file path.
 */
database_t		*mpm_database_open(u8_t *ret, const char *fn) {
	database_t		*ptr;
	u8_t			error = 0;

	ptr = malloc(sizeof(database_t));
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
u8_t			mpm_database_close(database_t *ptr) {
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
u8_t			mpm_database_exec(database_t *ptr, const char *query,
		SQL_CALLBACK_PTR(cl), void *ct, char **err) {
	if (ptr == NULL || query == NULL)
		return 1;

	return sqlite3_exec(ptr->sql, query, cl, ct, err);
}

/**
 * int name(void *context, int col_num, char **col_txt, char **col_name)
 */
SQL_CALLBACK_DEF(callback_package) {
	mlist_t		**head = context;
	package_t	*ptr;

	ptr = malloc(sizeof(package_t));
	assert(ptr != NULL);
	mpm_package_init(ptr);

	for (u8_t i = 0; i < col_num; i++)
		sql_to_package(ptr, col_name[i], col_txt[i]);

	list_add(*(head), ptr, sizeof(package_t));
	free(ptr);
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
u8_t			mpm_get_package_by_id(database_t *ptr, u64_t id,
						mlist_t **pkg) {
	char	*query;
	u8_t	ret;

	if (ptr == NULL)
		return 1;

	*pkg = NULL;
	asprintf(&query, QUERY_GET_PACKAGE_BY_ID(id));
	ret = sqlite3_exec(ptr->sql, query, &callback_package, pkg, NULL);
	free(query);
	return ret;
}

/*!
 * \brief Get a package by his name
 * \param ptr Opened Database connection
 * \param name Name to search for
 * \param pkg Pointer on a list, used to store the results
 * \return Error code
 *
 * This function will search in an already opened database package(s) with a
 * given name.
 * A sql QUERY is constructed in this function, with the following content:
 * SELECT * FROM pkgs WHERE name = %s, where %s is the given name
 * This function will call list_add to add results to the given list,
 * caller should properly free this list.
 *
 * \note This function will set pkg to NULL before filling it with the results.
 * You should not call this function with an existing package list.
 */
u8_t			mpm_get_package_by_name(database_t *ptr, const char *name,
						mlist_t **pkg) {
	char	*query;
	u8_t	ret;

	if (ptr == NULL)
		return 1;

	*pkg = NULL;
	asprintf(&query, QUERY_GET_PACKAGE_BY_NAME(name));
	ret = sqlite3_exec(ptr->sql, query, &callback_package, pkg, NULL);
	free(query);
	return ret;
}


/*!
 * \brief Fill a package_t structure with a SQL result
 * \param ptr Pointer to package_t. Must not be NULL.
 * \param name Name of the column
 * \param val Value of the column
 *
 * This function will transform an SQL result, given through an sqlite callback,
 * and fill a package_t structure with it.
 * All the types conversion needed (string -> *) are done in this function.
 *
 * \note If a unknown column is passed to this function, a panic will be throwed.
 */
package_t		*sql_to_package(package_t *ptr, char *name, char *val) {
	if (ptr == NULL)
		return ptr;

	if (strcmp(name, PKG_COL_ID) == 0) {
		ptr->id = strtoull(val, (char **)NULL, 10);
	} else if (strcmp(name, PKG_COL_NAME) == 0) {
			/*strcmp(name, PKG_COL_VERSION) == 0 ||*/
			/*strcmp(name, PKG_COL_DESC) == 0) {*/
		ptr->name = strdup(val);
	} else if (strcmp(name, PKG_COL_STATE) == 0) {
		ptr->state = val[0] - '0';
	} /*else {*/
		/*m_panic("Unknown column '%s' in get_package\n", name);*/
	/*}*/
	return ptr;
}

/*!
 * \brief Init an empty database
 * \param ptr Open connection to a database
 *
 * This function will create tables in an empty database.
 * Careful, this function do not check if a database is empty before making
 * requests.
 * If an error happens, m_error is called, and an appropriate return code is
 * returned.
 *
 * \return Status code
 */
u8_t		mpm_database_init(database_t *ptr) {
	static const char	*query_table[] = {									\
		/* Package table */
		SQL_CREATE_TABLE PKG_TABLE "("										\
			PKG_COL_ID			SQL_TYPE_INT	SQL_TYPE_PRIMARY_KEY	"," \
			PKG_COL_NAME		SQL_TYPE_TEXT	SQL_TYPE_NOT_NULL		"," \
			PKG_COL_VERSION		SQL_TYPE_TEXT	SQL_TYPE_NOT_NULL		"," \
			PKG_COL_CATEG		SQL_TYPE_TEXT	SQL_TYPE_NOT_NULL		"," \
			PKG_COL_DESC		SQL_TYPE_TEXT	SQL_TYPE_NOT_NULL		"," \
			PKG_COL_STATE		SQL_TYPE_INT	SQL_TYPE_NOT_NULL		"," \
			PKG_COL_DEPS		SQL_TYPE_TEXT							"," \
			PKG_COL_FILES		SQL_TYPE_TEXT	SQL_TYPE_NOT_NULL		"," \
			PKG_COL_BINARIES	SQL_TYPE_TEXT							"," \
			PKG_COL_CONFIG		SQL_TYPE_TEXT							"," \
			PKG_COL_DOCS		SQL_TYPE_TEXT								\
		");",																\
		/* Files table */
		SQL_CREATE_TABLE FILE_TABLE "("										\
			FILE_COL_ID			SQL_TYPE_INT	SQL_TYPE_PRIMARY_KEY	"," \
			FILE_COL_PATH		SQL_TYPE_TEXT	SQL_TYPE_NOT_NULL		"," \
			FILE_COL_TYPE		SQL_TYPE_INT	SQL_TYPE_NOT_NULL		"," \
			FILE_COL_PARENT		SQL_TYPE_INT	SQL_TYPE_NOT_NULL		"," \
			FILE_COL_PARENT_NAME SQL_TYPE_TEXT	SQL_TYPE_NOT_NULL		"," \
			FILE_COL_HASH		SQL_TYPE_TEXT	SQL_TYPE_NOT_NULL			\
		");",																\
		/* Category table */
		SQL_CREATE_TABLE CAT_TABLE "("										\
			CAT_COL_ID			SQL_TYPE_INT	SQL_TYPE_PRIMARY_KEY	"," \
			CAT_COL_NAME		SQL_TYPE_TEXT	SQL_TYPE_NOT_NULL		"," \
			CAT_COL_PARENT		SQL_TYPE_INT	SQL_TYPE_NOT_NULL		"," \
			CAT_COL_PARENT_NAME	SQL_TYPE_TEXT	SQL_TYPE_NOT_NULL			\
		");"
	};
	char	*err = NULL;
	u8_t	ret = 0;

	if (ptr == NULL)
		return 1;

	for (u8_t i = 0; i < sizeof(query_table) / sizeof(query_table[0]); i++) {
		ret = mpm_database_exec(ptr, query_table[i], NULL, NULL, &err);
		if (ret != 0 || err != NULL)
			goto error;
	}

	return 0;

error:
	m_error("An error happened in the database init: %s\n", err);
	sqlite3_free(err);
	return ret;
}

/*!
 * \brief Add a package entry in the database
 * \param ptr Opened connection to a database
 * \param pkg Package to add
 *
 * This function will add a package entry to an already opened connection.
 */
u8_t		mpm_database_add_pkg(database_t *ptr, package_t *pkg) {
	char	*query, *err;
	char	*deps, *files, *binaries, *config, *docs;
	u8_t	ret;

	if (pkg == NULL || ptr == NULL)
		return 1;

	deps = NULL;
	files = NULL;
	binaries = NULL;
	config = NULL;
	docs = NULL;
	asprintf(&query, SQL_INSERT_TABLE PKG_TABLE \
		" (%s, %s, %s, %s, %s, %s, %s, %s, %s, %s) " \
		"VALUES (\"%s\", \"%s\", \"%s\", \"%s\", %d, \"%s\", \"%s\", \"%s\", \"%s\", \"%s\");",
			PKG_COL_NAME, PKG_COL_VERSION, PKG_COL_CATEG, PKG_COL_DESC,
			PKG_COL_STATE, PKG_COL_DEPS, PKG_COL_FILES, PKG_COL_BINARIES,
			PKG_COL_CONFIG, PKG_COL_DOCS,
			pkg->name, pkg->version, pkg->categ->name, pkg->desc, 
			pkg->state, deps, files, binaries, config, docs);

	ret = mpm_database_exec(ptr, query, NULL, NULL, &err);
	free(query);
	assert(ret == 0 && err == NULL);
	sqlite3_free(err);
	return ret;
}

/*!
 * \brief Get a file by his Id
 * \param ptr Opened Database connection
 * \param id ID to search for
 * \param files Pointer on a list, used to store the results
 * \return Error code
 *
 * This function will search in an already opened database a file with a
 * given id.
 * A sql QUERY is constructed in this function, with the following content:
 * SELECT * FROM files WHERE id = %d, where %d is the given id
 * This function will call list_add to add results to the given list,
 * caller should properly free this list.
 *
 * \note This function will set files to NULL before filling it with the results.
 * You should not call this function with an existing files list.
 */
u8_t			mpm_get_file_by_id(database_t *ptr, u64_t id,
						mlist_t **files) {
	char	*query;
	u8_t	ret;

	if (ptr == NULL)
		return 1;

	*files = NULL;
	asprintf(&query, QUERY_GET_FILES_BY_ID(id));
	ret = sqlite3_exec(ptr->sql, query, &callback_files, files, NULL);
	free(query);
	return ret;
}

/*!
 * \brief Get a file by his path
 * \param ptr Opened Database connection
 * \param path Path to search for
 * \param files Pointer on a list, used to store the results
 * \return Error code
 *
 * This function will search in an already opened database a file with a
 * given path.
 * A sql QUERY is constructed in this function, with the following content:
 * SELECT * FROM files WHERE path = "%s", where %s is the given path
 * This function will call list_add to add results to the given list,
 * caller should properly free this list.
 *
 * \note This function will set files to NULL before filling it with the results.
 * You should not call this function with an existing files list.
 */
u8_t			mpm_get_file_by_path(database_t *ptr, const char *path,
						mlist_t **files) {
	char	*query;
	u8_t	ret;

	if (ptr == NULL || path == NULL)
		return 1;

	*files = NULL;
	asprintf(&query, QUERY_GET_FILES_BY_PATH(path));
	ret = sqlite3_exec(ptr->sql, query, &callback_files, files, NULL);
	free(query);
	return ret;
}

/*!
 * \brief Get a file by his parent id
 * \param ptr Opened Database connection
 * \param id Parent ID to search for
 * \param files Pointer on a list, used to store the results
 * \return Error code
 *
 * This function will search in an already opened database a file with a
 * given parent id.
 * A sql QUERY is constructed in this function, with the following content:
 * SELECT * FROM files WHERE parent_id = %d, where %d is the given id
 * This function will call list_add to add results to the given list,
 * caller should properly free this list.
 *
 * \note This function will set files to NULL before filling it with the results.
 * You should not call this function with an existing files list.
 */
u8_t			mpm_get_file_by_parent_id(database_t *ptr, u64_t id,
						mlist_t **files) {
	char	*query;
	u8_t	ret;

	if (ptr == NULL)
		return 1;

	*files = NULL;
	asprintf(&query, QUERY_GET_FILES_BY_PARENT_ID(id));
	ret = sqlite3_exec(ptr->sql, query, &callback_files, files, NULL);
	free(query);
	return ret;
}

/*!
 * \brief Get a file by his parent name
 * \param ptr Opened Database connection
 * \param name Parent name
 * \param files Pointer on a list, used to store the results
 * \return Error code
 *
 * This function will search in an already opened database a file with a
 * given parent name.
 * A sql QUERY is constructed in this function, with the following content:
 * SELECT * FROM files WHERE parent_name = "%s", where %s is the given parent name
 * This function will call list_add to add results to the given list,
 * caller should properly free this list.
 *
 * \note This function will set files to NULL before filling it with the results.
 * You should not call this function with an existing files list.
 */
u8_t			mpm_get_file_by_parent_name(database_t *ptr, const char *name,
						mlist_t **files) {
	char	*query;
	u8_t	ret;

	if (ptr == NULL || name == NULL)
		return 1;

	*files = NULL;
	asprintf(&query, QUERY_GET_FILES_BY_PARENT_NAME(name));
	ret = sqlite3_exec(ptr->sql, query, &callback_files, files, NULL);
	free(query);
	return ret;
}


/**
 * int name(void *context, int col_num, char **col_txt, char **col_name)
 */
SQL_CALLBACK_DEF(callback_files) {
	mlist_t		**head = context;
	file_t		*ptr;

	ptr = malloc(sizeof(file_t));
	assert(ptr != NULL);
	mpm_file_init(ptr);

	for (u8_t i = 0; i < col_num; i++)
		sql_to_file(ptr, col_name[i], col_txt[i]);

	list_add(*(head), ptr, sizeof(file_t));
	free(ptr);
	return 0;
}

/*!
 * \brief Fill a file_t structure with a SQL result
 * \param ptr Pointer to file_t. Must not be NULL.
 * \param name Name of the column
 * \param val Value of the column
 *
 * This function will transform an SQL result, given through an sqlite callback,
 * and fill a file_t structure with it.
 * All the types conversion needed (string -> *) are done in this function.
 *
 * \note If a unknown column is passed to this function, a panic will be throwed.
 */
file_t		*sql_to_file(file_t *ptr, char *name, char *val) {
	if (ptr == NULL)
		return ptr;

	if (strcmp(name, FILE_COL_ID) == 0) {
		ptr->id = strtoull(val, (char **)NULL, 10);
	} else if (strcmp(name, FILE_COL_PATH) == 0) {
		ptr->path = strdup(val);
	} else if (strcmp(name, FILE_COL_TYPE) == 0) {
		ptr->type = val[0] - '0';
	} else if (strcmp(name, FILE_COL_PARENT) == 0) {
		ptr->parent = NULL;
	} else if (strcmp(name, FILE_COL_PARENT_NAME) == 0) {
		ptr->parent_name = strdup(val);
	} else if (strcmp(name, FILE_COL_HASH) == 0) {
		ptr->hash = strdup(val);
	} else {
		m_panic("Unknown column '%s' in get_file\n", name);
	}
	return ptr;
}

/*!
 * \brief Add a file entry to the database
 * \param ptr Opened connection to a database
 * \param file File to add
 */
u8_t		mpm_database_add_file(database_t *ptr, file_t *file) {
	char	*query, *err;
	u8_t	ret;

	if (ptr == NULL || file == NULL)
		return 1;

	asprintf(&query, SQL_INSERT_TABLE FILE_TABLE \
		" (%s, %s, %s, %s, %s) " \
		"VALUES (\"%s\", \"%d\", \"%lld\", \"%s\", \"%s\");",
		FILE_COL_PATH, FILE_COL_TYPE, FILE_COL_PARENT, FILE_COL_PARENT_NAME,
		FILE_COL_HASH,
		file->path, file->type, file->parent->id, file->parent_name, file->hash);
	ret = mpm_database_exec(ptr, query, NULL, NULL, &err);
	free(query);
	assert(ret == 0 && err == NULL);
	sqlite3_free(err);
	return ret;
}

/*!
 * \brief Add a category in the database
 * \param ptr Opened connection to a database
 * \param cat Category to add
 */
u8_t		mpm_database_add_categ(database_t *ptr, category_t *cat) {
	char	*query, *err;
	u8_t	ret;

	if (ptr == NULL || cat == NULL)
		return 1;

	asprintf(&query, SQL_INSERT_TABLE CAT_TABLE \
		"(%s, %s, %s) " \
		"VALUES (\"%s\", \"%lld\", \"%s\");",
		CAT_COL_NAME, CAT_COL_PARENT, CAT_COL_PARENT_NAME,
		cat->name, cat->parent->id, cat->parent->name
	);
	ret = mpm_database_exec(ptr, query, NULL, NULL, &err);
	free(query);
	assert(ret == 0 && err == NULL);
	sqlite3_free(err);
	return ret;
}

/*!
 * \brief Get a category by his Id
 * \param ptr Opened Database connection
 * \param id ID to search for
 * \param files Pointer on a list, used to store the results
 * \return Error code
 *
 * This function will search in an already opened database a category with a
 * given id.
 * A sql QUERY is constructed in this function, with the following content:
 * SELECT * FROM categ WHERE id = %d, where %d is the given id
 * This function will call list_add to add results to the given list,
 * caller should properly free this list.
 *
 * \note This function will set files to NULL before filling it with the results.
 * You should not call this function with an existing files list.
 */
u8_t		mpm_get_categ_by_id(database_t *ptr, u64_t id, mlist_t **cat) {
	char	*query;
	u8_t	ret;

	if (ptr == NULL)
		return 1;

	*cat = NULL;
	asprintf(&query, QUERY_GET_CATEG_BY_ID(id));
	ret = sqlite3_exec(ptr->sql, query, &callback_categ, cat, NULL);
	free(query);
	return ret;
}

/**
 * int name(void *context, int col_num, char **col_txt, char **col_name)
 */
SQL_CALLBACK_DEF(callback_categ) {
	mlist_t		**head = context;
	category_t	*ptr;

	ptr = malloc(sizeof(category_t));
	assert(ptr != NULL);
	mpm_category_init(ptr);

	for (u8_t i = 0; i < col_num; i++)
		ptr = sql_to_category(ptr, col_name[i], col_txt[i]);

	list_add(*(head), ptr, sizeof(category_t));
	free(ptr);
	return 0;
}

/*!
 * \brief Fill a category_t structure with a SQL result
 * \param ptr Pointer to category_t. Must not be NULL.
 * \param name Name of the column
 * \param val Value of the column
 *
 * This function will transform an SQL result, given through an sqlite callback,
 * and fill a category_t structure with it.
 * All the types conversion needed (string -> *) are done in this function.
 *
 * \note If a unknown column is passed to this function, a panic will be throwed.
 */
category_t	*sql_to_category(category_t *ptr, char *name, char *val) {
	if (ptr == NULL)
		return ptr;

	if (strcmp(name, CAT_COL_ID) == 0) {
		ptr->id = strtoull(val, (char **)NULL, 10);
	} else if (strcmp(name, CAT_COL_NAME) == 0) {
		ptr->name = strdup(val);
	} else if (strcmp(name, CAT_COL_PARENT) == 0) {
		ptr->parent = NULL; /* TODO */
	} else if (strcmp(name, CAT_COL_PARENT_NAME) == 0) {
		ptr->parent_name = strdup(val);
	} else {
		m_panic("Unknown column '%s' in get_category\n", name);
	}
	return ptr;
}
