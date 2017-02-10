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
# define SQL_CALLBACK_DEF(name) int name(void *context, int col_num, \
                                        char **col_txt, char **col_name)

/* Package search queries */
# define QUERY_GET_PACKAGE_BY_ID(id)            "SELECT * FROM " \
                                                    PKG_TABLE \
                                                    " WHERE " PKG_COL_ID \
                                                    " = %lld", id

# define QUERY_GET_PACKAGE_BY_NAME(name)	"SELECT * FROM " \
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

typedef struct  s_database {
    sqlite3     *sql;
}               database_t;


/* SQL callbacks */
SQL_CALLBACK_DEF(callback_package);
SQL_CALLBACK_DEF(callback_files);
SQL_CALLBACK_DEF(callback_categ);

database_t      *mpm_database_open(u8_t *ret, const char *fn);
u8_t            mpm_database_close(database_t *ptr);
u8_t            mpm_database_exec(database_t *ptr, const char *query,
                    SQL_CALLBACK_PTR(cl), void *ct, char **err);
u8_t            mpm_get_package_by_id(database_t *ptr, u64_t id,
                    mlist_t **pkg);
u8_t            mpm_get_package_by_name(database_t *ptr, const char *name,
                    mlist_t **pkg);
package_t       *sql_to_package(package_t *ptr, char *name, char *val);
u8_t            mpm_database_init(database_t *ptr);
u8_t            mpm_database_add_pkg(database_t *ptr, package_t *pkg);
u8_t            mpm_get_file_by_id(database_t *ptr, u64_t id,
                    mlist_t **files);
u8_t            mpm_get_file_by_path(database_t *ptr, const char *path,
                    mlist_t **files);
u8_t            mpm_get_file_by_parent_id(database_t *ptr, u64_t id,
                    mlist_t **files);
u8_t            mpm_get_file_by_parent_name(database_t *ptr, const char *name,
                    mlist_t **files);
file_t          *sql_to_file(file_t *ptr, char *name, char *val);
u8_t            mpm_database_add_file(database_t *ptr, file_t *file);
category_t      *sql_to_category(category_t *ptr, char *name, char *val);
u8_t            mpm_get_categ_by_id(database_t *ptr, u64_t id, mlist_t **cat);
u8_t            mpm_get_categ_by_name(database_t *ptr, const char *name,
                    mlist_t **cat);
u8_t            mpm_database_add_categ(database_t *ptr, category_t *cat);

#endif /* DATABASE_H */
