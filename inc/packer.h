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

#ifndef PACKER_H
# define PACKER_H

# define _GNU_SOURCE
# include <sys/types.h>
# include <sys/stat.h>
# include <json.h>
# include <arpa/inet.h>
# include <sys/types.h>
# include <dirent.h>
# include <limits.h>
# include <packer_file.h>
# include <stdio.h>

# define PACKER_DEF_CONF_FN     "package.json"
# define PACKER_DEF_EXT         ".mpx"
# define PACKER_MPX_MAGIC       "MPX"

# define PACKER_SRC_DIR         "srcs/"
# define PACKER_PATCH_DIR       "patches/"
# define PACKER_SCRIPT_DIR      "scripts/"

# define PACKER_MAKE_DEF        "make"
# define PACKER_TEST_DEF        "make test"
# define PACKER_INST_DEF        "make install"

typedef enum packer_type_e {
    PACKER_TYPE_DIRECTORY,
    PACKER_TYPE_ARCHIVE
} packer_type_t;

# define PACKER_CONF_PACKAGE_TOKEN              "package"
# define PACKER_CONF_PACKAGE_NAME_TOKEN         "name"
# define PACKER_CONF_PACKAGE_VERSION_TOKEN      "version"
# define PACKER_CONF_PACKAGE_DESC_TOKEN         "description"
# define PACKER_CONF_PACKAGE_SBU_TOKEN          "SBU"
# define PACKER_CONF_PACKAGE_CATEG_TOKEN        "category"
# define PACKER_CONF_PACKAGE_INST_SIZE_TOKEN    "inst_size"
# define PACKER_CONF_COMP_TOKEN                 "compilation"
# define PACKER_CONF_COMP_CONF_TOKEN            "configure"
# define PACKER_CONF_COMP_MAKE_TOKEN            "make"
# define PACKER_CONF_COMP_TEST_TOKEN            "test"
# define PACKER_CONF_COMP_INST_TOKEN            "install"
# define PACKER_CONF_COMP_ENV_TOKEN              "env"
# define PACKER_CONF_DEPS_TOKEN                 "dependencies"

# define JSON_SKIP_NULL(obj, it)    if (json_object_get_type(obj) == json_type_null) \
                                { \
                                    json_object_iter_next(&(it)); \
                                    continue; \
                                } \

typedef struct packer_header_package_s {
    char    *name;        /*!< Name of the package */
    char    *version;     /*!< Version of the package */
    char    *description; /*!< Description of the package */
    double  _sbu;         /*!< SBU (Installation time) */
    char    *categ;       /*!< Category of the package */
    double  inst_size;    /*!< Installation size */
} packer_header_package_t;

typedef struct packer_conf_opt_s {
    char    *name;  /*!< Name of the option (can be NULL) */
    char    *value; /*!< Value of the option */
} packer_conf_opt_t;

typedef struct packer_header_comp_s {
    mlist_t     *configure; /*!< Configure options */
    char        *make;      /*!< Make options */
    char        *test;      /*!< Test instructions */
    char        *install;   /*!< Install instructions */
    mlist_t     *env;       /*!< Environnement variables */
} packer_header_comp_t;

typedef struct packer_header_deps_s {
    mlist_t     *list; /*!< List of strings, containing the dependencies */
} packer_header_deps_t;

typedef struct packer_header_s {
    packer_header_package_t *package;      /*!< Package header section */
    packer_header_comp_t    *compilation;  /*!< Compilation header section */
    packer_header_deps_t    *dependencies; /*!< Package dependencies */
} packer_header_t;

typedef struct packer_s {
    char            *str;    /*!< Directory, or path of the package */
    json_object     *json;   /*!< JSON object */
    packer_type_t   type;    /*!< Type of packer */
    packer_header_t *header; /*!< Header of the MPX format */
    char            *out_dir;/*!< Output directory, in case of an archive */
} packer_t;


# include <libmpm.h>

/*!
 * \brief Allocate, fill and init a packer_t structure
 *
 * \param[in] dir Directory of the package
 *
 * \return A freshly allocated pointer on success, NULL on failure
 */
packer_t *packer_init_dir(const char *dir);

/*!
 * \brief Allocate, fill an init a packer_t structure
 *
 * \param[in] path Path of the archive
 *
 * \return A freshly allocated pointer on success, NULL on failure
 */
packer_t *packer_init_archive(const char *path);

/*!
 * \brief Free a packer_t structure
 *
 * \param[in,out] ptr Pointer to free
 */
void packer_free(packer_t *ptr);

/*!
 * \brief Read a directory and prepare the packer
 *
 * \param[in,out] ctx Already initialized packer_t struct
 *
 * \note This function is only for directory TO pack
 * \note Throw an assertion if the passed pointer is NULL
 *
 * \return true on success, false on failure
 */
bool packer_read_dir(packer_t *ctx);

/*!
 * \brief Create a .mpx archive with a directory
 *
 * \param[in] ctx Already initialized packer_t struct
 * \param[in] archive_path Filename of the output
 *
 * Create a .mpx archive with archive with an already parsed directory.
 *
 * \note This function is not NULL safe against ctx and archive_path parameters,
 * it will raise an assertion
 *
 * \return True on success, false on failure
 */
bool packer_create_archive(packer_t *ctx, const char *archive_path);

/*!
 * \brief Read a .mpx archive, and stock it in memory
 *
 * \param[in] ctx Already initialized packer_t struct
 *
 * Read an .mpx archive in order to extract compilation informations off it
 *
 * \return True on success, false on failure
 */
bool packer_read_archive_in_memory(packer_t *ctx);

/*!
 * \brief Read the header of a .mpx archive, and stock it in memory
 *
 * \param[in,out] ctx Already initialized packer_t struct
 *
 * \return True on success, false on failure
 */
bool packer_read_archive_header(packer_t *ctx);

/*!
 * \brief Extract an archive in a directory
 *
 * \param[in] ctx Archive to extract
 * \param[in] dir Parent directory to extract to
 *
 * This function will extract an MPX archive in a designated dir
 * It will create a main directory too, with the format NAME-VERSION
 * For example, if the call is:
 *      packer_extract_archive(ctx, "/tmp");
 * The output directory can be found in the member out_dir in packer_t
 *
 * \return true on success, false on failure
 */
bool packer_extract_archive(packer_t *ctx, const char *dir);

#endif /* PACKER_H */
