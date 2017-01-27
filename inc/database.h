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

# define DB_FN "test.db"

# define SQL_CALLBACK_PTR(name) int (*name)(void *, int, char**, char**)
# define SQL_CALLBACK_DEF(name) int name(void *context, int col_num, char **col_txt, char **col_name)

typedef struct s_database {
	sqlite3		*sql;
}				mdatabase_t;


mdatabase_t		*mpm_database_open(u8_t *ret, const char *fn);
u8_t			mpm_database_close(mdatabase_t *ptr);
u8_t			mpm_database_exec(mdatabase_t *ptr, const char *query,
		SQL_CALLBACK_PTR(cl), void *ct, char **err);

#endif /* DATABASE_H */
