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

#ifndef PACKER_FILE_H
# define PACKER_FILE_H

# include <morphux.h>
# include <sodium.h>
# include <zlib.h>
# include <sys/types.h>
# include <dirent.h>

typedef struct packer_file_s {
    char    *fn;                           /*!< Filename, with relative path (Ex: srcs/main.c ) */
    off_t   file_size;                     /*!< File size, in bytes */
    off_t   compressed_size;               /*!< Compression size, in bytes */
    char    sum[crypto_hash_sha256_BYTES]; /*!< Sha256sum of the file */
    char    *content;                      /*!< Actual content of the file */
} packer_file_t;

/*!
 * \brief Free a packer_file_t structure
 *
 * \param[in,out] magic Pointer to the struct
 *
 * \note Can be used as a list_free callback
 *
 * \return 1 on success, 0 on failure
 */
int packer_file_free(void *magic);

/*!
 * \brief Allocate and init a packer_file_t struct
 *
 * \param[in] file File name
 * \param[in] dir Parent directory
 *
 * \return Freshly allocated structure on success, NULL on failure
 */
packer_file_t *packer_file_init(const char *file, const char *dir);

/*!
 * \brief Fill a packer_file_t list with a directory
 *
 * \param[in] dir_name Directory name
 * \param[in,out] files Pointer to the head of a list
 * \param[out] dirs Pointer to a list of directory
 *
 * This functions will read each member inside a directory.
 * If it's a file, this functions will call get_file_information on it
 * If it's a directory, the function will add the directory name (full path)
 * in the dirst linked list
 *
 * \sa get_file_information
 * \return true on success, false on failure
 */
bool read_files_from_dir(const char *dir_name, mlist_t **files, mlist_t **dirs);

/*!
 * \brief Get informations about a file
 *
 * \param[in,out] file Already filled packer_file_t structure
 *
 * This function will read the file specified in the member fn of the given
 * structure, and get some informations about it:
 *  - His size
 *  - His content (compressed)
 *  - The Sha256sum of the content, uncompressed
 *
 * \return true on success, false on failure
 */
bool get_file_information(packer_file_t *file);

/*!
 * \brief Read a file from a binary archive
 *
 * \param[in] content Binary content
 * \param[in,out] ctr Cursor
 *
 * Read a file from binary stream, from the cursor position
 *
 * \return A freshly filled and allocated packer_file_t struct on success,
 * NULL on failure
 */
bool packer_file_from_binary_to_disk(const char *content, off_t *ctr);

/*!
 * \brief Write a file to disk
 *
 * \param[in] file File to write
 *
 * \return true on success, false on failure
 */
FILE *packer_file_to_disk(packer_file_t *file);

#endif /* PACKER_FILE_H */
