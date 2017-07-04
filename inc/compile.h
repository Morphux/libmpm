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

#ifndef COMPILE_H
# define COMPILE_H

# include <packer.h>

# define COMP_BEFORE_SCRIPT PACKER_SCRIPT_DIR "before.sh"
# define COMP_AFTER_SCRIPT PACKER_SCRIPT_DIR "after.sh"

# define PATCH_CMD              "patch -Ni"
# define CONFIGURE_CMD          "./configure"

# define DEFAULT_EXTRACT_DIR    "/var/tmp/mpm_private"

typedef enum {
    INST_STATE_NONE = 0,
    INST_STATE_BEFORE,
    INST_STATE_PATCHING,
    INST_STATE_CONFIGURATION,
    INST_STATE_COMPILATION,
    INST_STATE_INSTALLATION,
    INST_STATE_AFTER,
    INST_STATE_DB,
    INST_STATE_DONE,
    INST_STATE_LAST
} install_state_t;

typedef struct compile_s {
    packer_t            *package;          /*!< Pointer to the package */
    char                old_pwd[PATH_MAX]; /*!< Old processs path */
    install_state_t     state;             /*!< State of the installation */
} compile_t;


/*!
 * \brief Init a compile_t structure, in order to prepare installation
 *
 * \param[in] ctx Package to install
 *
 * This function will create a new compile_t structure, save the current pwd,
 * and chdir in the decompressed directory of the package.
 *
 * \note This function take the ownership of the packer_t pointer
 * \return A freshly allocated structure on success, NULL on failure
 */
compile_t *package_install_init(packer_t *ctx);

/*!
 * \brief Cleanup after an installation
 *
 * \param[in] ctx Installation to cleanup
 *
 * This function will chdir into the old directory, and free all the structure
 *
 * \note If the function fails to chdir in the old directory, data will not be
 * freed
 * \return true on success, false on failure
 */
bool package_install_cleanup(compile_t *ctx);

/*!
 * \brief Execute before script, if there is any
 * 
 * \param[in] ctx Current package
 *
 * \return true on success, false on failure
 */
bool before_package(compile_t *ctx);

/*!
 * \brief Apply system and user patch
 *
 * \param[in] ctx Current package
 *
 * \return true on succes, false on failure
 */
bool patch_package(compile_t *ctx);

/*!
 * \brief Configure a package
 *
 * \param[in] ctx Package to configure
 *
 * \return true on success, false on failure
 */
bool configure_package(compile_t *ctx);

/*!
 * \brief Compile a package
 *
 * \param[in] ctx Package to compile
 *
 * \return true on success, false on failure
 */
bool make_package(compile_t *ctx);

/*!
 * \brief Install a Package
 *
 * \param[in] ctx Package to compile
 *
 * \return true on success, false on failure
 */
bool install_package(compile_t *ctx);

/*!
 * \brief Execute post-installation script
 *
 * \param[in] ctx Current package
 *
 * \return truc on success, false on failure
 */
bool after_package(compile_t *ctx);

/*!
 * \brief Extract and Install an .mpx archive
 *
 * \param[in] ctx Archive to install
 * \param[in] ptr Pointer to compile_t pointer
 *
 * \return true on success, false on failure
 */
bool install_archive(packer_t *ctx, compile_t **ptr);

/*!
 * \brief install_state_t to human readable string
 *
 * \param num Install state
 *
 * \return String
 */
const char *install_state_to_str(install_state_t num);

#endif /* COMPILE_H */
