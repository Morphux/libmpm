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

#ifndef DATABASE_H
# define DATABASE_H
# include <sqlite3.h>
# include <morphux.h>
# include <files.h>
# include <package.h>

# define DB_FN "test.db"
# define SQL_CALLBACK_PTR(name) int (*name)(void *, int, char**, char**)
# define SQL_CALLBACK_DEF(name) static int name(void *context, int col_num, \
                                        char **col_txt, char **col_name)

/* Package search queries */
# define QUERY_GET_PACKAGE_BY_ID(id)            "SELECT * FROM " \
                                                    PKG_TABLE \
                                                    " WHERE " PKG_COL_ID \
                                                    " = %lld", id

# define QUERY_GET_PACKAGE_BY_NAME(name)        "SELECT * FROM " \
                                                    PKG_TABLE \
                                                    " WHERE " PKG_COL_NAME \
                                                    " = \"%s\"", name

/* File search queries */
# define QUERY_GET_FILES_BY_ID(id)              "SELECT * FROM " \
                                                    FILE_TABLE \
                                                    " WHERE " FILE_COL_ID \
                                                    " = %lld", id

# define QUERY_GET_FILES_BY_PATH(path)          "SELECT * FROM " \
                                                    FILE_TABLE \
                                                    " WHERE " FILE_COL_PATH \
                                                    " = \"%s\"", path

# define QUERY_GET_FILES_BY_PARENT_ID(id)       "SELECT * FROM " \
                                                    FILE_TABLE \
                                                    " WHERE " FILE_COL_PARENT \
                                                    " = %lld", id

# define QUERY_GET_FILES_BY_PARENT_NAME(name)   "SELECT * FROM " \
                                                    FILE_TABLE \
                                                    " WHERE " FILE_COL_PARENT_NAME \
                                                    " = \"%s\"", name

/* Category search queries */
# define QUERY_GET_CATEG_BY_ID(id)              "SELECT * FROM " \
                                                    CAT_TABLE \
                                                    " WHERE " CAT_COL_ID \
                                                    " = %lld", id

# define QUERY_GET_CATEG_BY_NAME(name)          "SELECT * FROM " \
                                                    CAT_TABLE \
                                                    " WHERE " CAT_COL_NAME \
                                                    " = \"%s\"", name

# define SQL_CREATE_TABLE               "CREATE table "
# define SQL_INSERT_TABLE               "INSERT INTO "
# define SQL_TYPE_PRIMARY_KEY           " primary key "
# define SQL_TYPE_NOT_NULL              " not null "
# define SQL_TYPE_INT                   " integer "
# define SQL_TYPE_TEXT                  " text "

typedef struct  database_s {
    sqlite3     *sql;
}               database_t;


/*!
 * \brief Open a connection to a database
 * \param[out] ret Return code, if any error
 * \param[in] fn Database path
 *
 * This function will create a new database_t struct, allocate it, and open
 * a new database connection. In case of any error, the return value will be
 * NULL, and the ret pointer set to an error code.
 * If a value is passed to the param fn, the library will use this value as a 
 * database file path.
 * \return A fresh database_t structure
 */
database_t      *mpm_database_open(u8_t *ret, const char *fn);

/*!
 * \brief Close a connection to an existing database
 * \param[in] ptr An open database
 *
 * This function will close a database connection and free the passed struct.
 * On any error, this function will return the error code.
 * \return The error code
 */
u8_t            mpm_database_close(database_t *ptr);

/*!
 * \brief Execute a SQL query on an already opened database
 * \param[in] ptr Opened database
 * \param[in] query SQL Query
 * \param[in] cl Query callback
 * \param[in,out] ct First parameter of the callback (Context)
 * \param[out] err Pointer on a string, to be filled with a string error.
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
u8_t            mpm_database_exec(database_t *ptr, const char *query,
                    SQL_CALLBACK_PTR(cl), void *ct, char **err);
/*!
 * \brief Get a package by his Id
 * \param[in] ptr Opened Database connection
 * \param[in] id ID to search for
 * \param[out] pkg Pointer on a list, used to store the results
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
u8_t            mpm_get_package_by_id(database_t *ptr, u64_t id,
                    mlist_t **pkg);
/*!
 * \brief Get a package by his name
 * \param[in] ptr Opened Database connection
 * \param[in] name Name to search for
 * \param[out] pkg Pointer on a list, used to store the results
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
u8_t            mpm_get_package_by_name(database_t *ptr, const char *name,
                    mlist_t **pkg);
/*!
 * \brief Fill a package_t structure with a SQL result
 * \param[in] ptr Pointer to package_t. Must not be NULL.
 * \param[in] name Name of the column
 * \param[in] val Value of the column
 *
 * This function will transform an SQL result, given through an sqlite callback,
 * and fill a package_t structure with it.
 * All the types conversion needed (string -> *) are done in this function.
 *
 * \note If a unknown column is passed to this function, a panic will be throwed.
 */
package_t       *sql_to_package(package_t *ptr, char *name, char *val);

/*!
 * \brief Init an empty database
 * \param[in] ptr Open connection to a database
 *
 * This function will create tables in an empty database.
 * Careful, this function do not check if a database is empty before making
 * requests.
 * If an error happens, m_error is called, and an appropriate return code is
 * returned.
 *
 * \return Status code
 */
u8_t            mpm_database_init(database_t *ptr);

/*!
 * \brief Add a package entry in the database
 * \param[in] ptr Opened connection to a database
 * \param[in] pkg Package to add
 *
 * This function will add a package entry to an already opened connection.
 */
u8_t            mpm_database_add_pkg(database_t *ptr, package_t *pkg);

/*!
 * \brief Get a file by his Id
 * \param[in] ptr Opened Database connection
 * \param[in] id ID to search for
 * \param[out] files Pointer on a list, used to store the results
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
 * \return Error code
 */
u8_t            mpm_get_file_by_id(database_t *ptr, u64_t id,
                    mlist_t **files);

/*!
 * \brief Get a file by his path
 * \param[in] ptr Opened Database connection
 * \param[in] path Path to search for
 * \param[out] files Pointer on a list, used to store the results
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
 * \return Error code
 */
u8_t            mpm_get_file_by_path(database_t *ptr, const char *path,
                    mlist_t **files);

/*!
 * \brief Get a file by his parent id
 * \param[in] ptr Opened Database connection
 * \param[in] id Parent ID to search for
 * \param[out] files Pointer on a list, used to store the results
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
 * \return Error code
 */
u8_t            mpm_get_file_by_parent_id(database_t *ptr, u64_t id,
                    mlist_t **files);

/*!
 * \brief Get a file by his parent name
 * \param[in] ptr Opened Database connection
 * \param[in] name Parent name
 * \param[out] files Pointer on a list, used to store the results
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
 * \return Error code
 */
u8_t            mpm_get_file_by_parent_name(database_t *ptr, const char *name,
                    mlist_t **files);

/*!
 * \brief Fill a file_t structure with a SQL result
 * \param[in] ptr Pointer to file_t. Must not be NULL.
 * \param[in] name Name of the column
 * \param[in] val Value of the column
 *
 * This function will transform an SQL result, given through an sqlite callback,
 * and fill a file_t structure with it.
 * All the types conversion needed (string -> *) are done in this function.
 *
 * \note If a unknown column is passed to this function, a panic will be throwed.
 */
file_t          *sql_to_file(file_t *ptr, char *name, char *val);

/*!
 * \brief Add a file entry to the database
 * \param[in] ptr Opened connection to a database
 * \param[in] file File to add
 */
u8_t            mpm_database_add_file(database_t *ptr, file_t *file);

/*!
 * \brief Fill a category_t structure with a SQL result
 * \param[in] ptr Pointer to category_t. Must not be NULL.
 * \param[in] name Name of the column
 * \param[in] val Value of the column
 *
 * This function will transform an SQL result, given through an sqlite callback,
 * and fill a category_t structure with it.
 * All the types conversion needed (string -> *) are done in this function.
 *
 * \note If a unknown column is passed to this function, a panic will be throwed.
 */
category_t      *sql_to_category(category_t *ptr, char *name, char *val);

/*!
 * \brief Get a category by his Id
 * \param[in] ptr Opened Database connection
 * \param[in] id ID to search for
 * \param[out] cat Pointer on a list, used to store the results
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
 * \return Error code
 */
u8_t            mpm_get_categ_by_id(database_t *ptr, u64_t id, mlist_t **cat);

/*!
 * \brief Get a category by his name
 * \param[in] ptr Opened Database connection
 * \param[in] name Name to search for
 * \param[out] cat Pointer on a list, used to store the results
 *
 * This function will search in an already opened database a category with a
 * given name.
 * A sql QUERY is constructed in this function, with the following content:
 * SELECT * FROM categ WHERE name = %s, where %s is the given name
 * This function will call list_add to add results to the given list,
 * caller should properly free this list.
 *
 * \note This function will set files to NULL before filling it with the results.
 * You should not call this function with an existing files list.
 * \return Error code
 */
u8_t            mpm_get_categ_by_name(database_t *ptr, const char *name,
                    mlist_t **cat);

/*!
 * \brief Add a category in the database
 * \param[in] ptr Opened connection to a database
 * \param[in] cat Category to add
 */
u8_t            mpm_database_add_categ(database_t *ptr, category_t *cat);

#endif /* DATABASE_H */
