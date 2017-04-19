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

# include <libmpm.h>
# include <sys/types.h>
# include <sys/stat.h>
# include <json.h>
# include <arpa/inet.h>
# include <sys/types.h>
# include <dirent.h>
# include <limits.h>
# include <sodium.h>

# define PACKER_DEF_CONF_FN     "package.json"
# define PACKER_DEF_EXT         ".mpx"
# define PACKER_MPX_MAGIC       "MPX"

# define PACKER_SRC_DIR         "srcs/"

# define PACKER_MAKE_DEF        "make"
# define PACKER_TEST_DEF        "test"
# define PACKER_INST_DEF        "install"

typedef enum packer_type_e {
    PACKER_TYPE_DIRECTORY,
    PACKER_TYPE_ARCHIVE
} packer_type_t;

# define PACKER_CONF_PACKAGE_TOKEN              "package"
# define PACKER_CONF_PACKAGE_NAME_TOKEN         "name"
# define PACKER_CONF_PACKAGE_VERSION_TOKEN      "version"
# define PACKER_CONF_PACKAGE_DESC_TOKEN         "description"
# define PACKER_CONF_COMP_TOKEN                 "compilation"
# define PACKER_CONF_COMP_CONF_TOKEN            "configure"
# define PACKER_CONF_COMP_MAKE_TOKEN            "make"
# define PACKER_CONF_COMP_TEST_TOKEN            "test"
# define PACKER_CONF_COMP_INST_TOKEN            "install"
# define PACKER_CONF_DEPS_TOKEN                 "dependencies"

typedef struct packer_header_package_s {
    char    *name;        /*!< Name of the package */
    char    *version;     /*!< Version of the package */
    char    *description; /*!< Description of the package */
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
} packer_t;

typedef struct packer_file_s {
    char    *fn;                           /*!< Filename, with relative path (Ex: srcs/main.c ) */
    off_t   file_size;                     /*!< File size, in bytes */
    char    sum[crypto_hash_sha256_BYTES]; /*!< Sha256sum of the file */
    char    *content;                      /*!< Actual content of the file */
} packer_file_t;

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
 * \brief Read a .mpx archive
 *
 * \param[in] ctx Already initialized packer_t struct
 *
 * Read an .mpx archive in order to extract compilation informations off it
 *
 * \return True on success, false on failure
 */
bool packer_read_archive(packer_t *ctx);

#endif /* PACKER_H */
