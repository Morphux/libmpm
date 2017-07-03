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

#include <compile.h>

/**
 * Compilation states, in string
 * @see: install_state_t enum for more information
 */
static const char *g_install_states_str[] = {
    __SET_ERR_STR(INST_STATE_NONE, "None"),
    __SET_ERR_STR(INST_STATE_BEFORE, "Before script"),
    __SET_ERR_STR(INST_STATE_PATCHING, "Patching"),
    __SET_ERR_STR(INST_STATE_CONFIGURATION, "Configuration"),
    __SET_ERR_STR(INST_STATE_COMPILATION, "Compilation"),
    __SET_ERR_STR(INST_STATE_INSTALLATION, "Installation"),
    __SET_ERR_STR(INST_STATE_AFTER, "After scripts"),
    __SET_ERR_STR(INST_STATE_DB, "Database operations"),
    __SET_ERR_STR(INST_STATE_DONE, "Done")
    /* Don't need a INST_STATE_LAST */
};

const char *install_state_to_str(install_state_t num) {
    const char *ret = NULL;

    /* Check for potential overflow */
    if (num >= 0 && num < INST_STATE_LAST)
        ret = g_install_states_str[num];
    return ret;
}

compile_t *package_install_init(packer_t *ctx) {
    compile_t           *ret = NULL;
    vector_string_t     *opt = NULL;
    mlist_t             *tmp = NULL;

    ret = malloc(sizeof(*ret));
    if (ret == NULL)
    {
        SET_ERR(ERR_MEMORY);
        return NULL;
    }

    ret->package = ctx;
    ret->state = INST_STATE_NONE;

    /* Saving the current pwd */
    getcwd(ret->old_pwd, sizeof(ret->old_pwd));

    /* Try to go to the extracted archive directory */
    if (chdir(ctx->out_dir) != 0)
    {
        free(ret);
        SET_ERR(ERR_CHDIR_FAILED);
        SET_ERR_STR_FMT("Cannot enter directory '%s'", ctx->out_dir);
        return NULL;
    }
    if (ctx->header->compilation.env == NULL)
        return ret;

    /* Set the environements variable, specified by the package */
    list_for_each(ctx->header->compilation.env, tmp, opt)
    {
        if (opt->str1 != NULL)
            setenv(opt->str2, opt->str1, 1);
        else
            setenv(opt->str2, "", 1);
    }

    return ret;
}

bool package_install_cleanup(compile_t *ctx) {
    if (ctx == NULL)
        return true;

    if (chdir(ctx->old_pwd) != 0)
    {
        SET_ERR(ERR_CHDIR_FAILED);
        SET_ERR_STR_FMT("Cannot enter directory '%s'", ctx->old_pwd);
        return false;
    }

    packer_free(ctx->package);
    free(ctx);
    return true;
}

bool before_package(compile_t *ctx) {
    /* Test the presence of the before script */
    if (file_exist(COMP_BEFORE_SCRIPT) == false)
        goto end;

    /* Exec the script */
    if (exec_line("sh " COMP_BEFORE_SCRIPT) != 0)
    {
        SET_ERR(ERR_EXEC_FAILED);
        SET_ERR_STR_FMT("Execution failed for '%s'", "sh " COMP_BEFORE_SCRIPT);
        return false;
    }

end:
    ctx->state = INST_STATE_BEFORE;
    return true;
}

bool patch_package(compile_t *ctx) {
    DIR             *dir = opendir(PACKER_PATCH_DIR);
    struct dirent   *dinfo = NULL;
    char            *cmd = NULL;
    bool            status = false;

    /* Nothing to patch, we're good */
    if (dir == NULL)
        return true;

    /* Trying to go in the srcs/ directory */
    if (chdir(PACKER_SRC_DIR) == -1)
    {
        SET_ERR(ERR_CHDIR_FAILED);
        SET_ERR_STR_FMT("Cannot enter directory '%s'", PACKER_SRC_DIR);
        goto end;
    }

    /* While on every patch */
    while ((dinfo = readdir(dir)))
    {
        /* Skip .* files */
        if (strlen(dinfo->d_name) > 0 && dinfo->d_name[0] == '.')
            continue ;

        /* Ignore directories */
        if (dinfo->d_type != DT_DIR)
        {
            /* Create the command */
            asprintf(&cmd, "%s ../%s%s", PATCH_CMD, PACKER_PATCH_DIR, dinfo->d_name);

            /* Execute the command */
            if (exec_line(cmd) != 0)
            {
                SET_ERR(ERR_EXEC_FAILED);
                SET_ERR_STR_FMT("Execution failed for '%s'", cmd);
                free(cmd);
                goto end;
            }

            free(cmd);
        }
    }

    /* Trying to go back in the main directory */
    if (chdir("..") == -1)
    {
        SET_ERR(ERR_CHDIR_FAILED);
        SET_ERR_STR("Cannot enter the directory '..'");
        goto end;
    }

    status = true;
end:
    closedir(dir);
    ctx->state = INST_STATE_PATCHING;
    return status;
}

bool configure_package(compile_t *ctx) {
    mlist_t             *cmd = NULL, *tmp;
    vector_string_t   *opt;
    char                *arg = NULL;
    bool                ret = false;

    /* Nothing to configure, we're good */
    if (ctx->package->header->compilation.configure == NULL)
    {
        ret = true;
        goto end;
    }

    /* Trying to go in the srcs/ directory */
    if (chdir(PACKER_SRC_DIR) == -1)
    {
        SET_ERR(ERR_CHDIR_FAILED);
        SET_ERR_STR_FMT("Cannot enter the directory '%s'", PACKER_SRC_DIR);
        goto end;
    }

    /* Since we don't know the size of the configure, we're using a list for it */
    list_add(cmd, CONFIGURE_CMD, sizeof(CONFIGURE_CMD) + 1);

    /* While on each parameter */
    list_for_each(ctx->package->header->compilation.configure, tmp, opt) {

        /* If the parameter got an argument */
        if (opt->str1 != NULL)
            asprintf(&arg, "--%s=%s", opt->str1, opt->str2);
        else
            asprintf(&arg, "--%s", opt->str2);

        /* Adding to the list */
        list_add(cmd, arg, strlen(arg) + 1);
        free(arg);
    }

    /* Execute the command (from a list) */
    if (exec_list(cmd) == 0)
        ret = true;
    else
    {
        SET_ERR(ERR_EXEC_FAILED);
        SET_ERR_STR_FMT("Execution failed for '%s'", cmd);
    }

    list_free(cmd, NULL);

    /* Go back on the main directory */
    chdir("..");

end:
    ctx->state = INST_STATE_CONFIGURATION;
    return ret;
}

bool make_package(compile_t *ctx) {
    bool status = true;

    /* Nothing to compile, we're good */
    if (STR_NULL_OR_EMPTY(ctx->package->header->compilation.make))
        goto end;

    /* Trying to go in the srcs/ directory */
    if (chdir(PACKER_SRC_DIR))
    {
        SET_ERR(ERR_CHDIR_FAILED);
        SET_ERR_STR_FMT("Cannot enter the directory '%s'", PACKER_SRC_DIR);
        return false;
    }

    /* Execute the makefile */
    if (exec_line(ctx->package->header->compilation.make) != 0)
    {
        SET_ERR(ERR_EXEC_FAILED);
        SET_ERR_STR_FMT("Execution failed for '%s'", ctx->package->header->compilation.make);
        status = false;
    }

    /* Go back to the main directory */
    chdir("..");

end:
    ctx->state = INST_STATE_COMPILATION;
    return status;
}

bool install_package(compile_t *ctx) {
    bool status = true;

    /* Nothing to install, we're good */
    if (STR_NULL_OR_EMPTY(ctx->package->header->compilation.install))
        goto end;

    /* Trying to go in the srcs/ directory */
    if (chdir(PACKER_SRC_DIR))
    {
        SET_ERR(ERR_CHDIR_FAILED);
        SET_ERR_STR_FMT("Cannot enter the directory '%s'", PACKER_SRC_DIR);
        return false;
    }

    /* Execute the installation command line */
    if (exec_line(ctx->package->header->compilation.install) != 0)
    {
        SET_ERR(ERR_EXEC_FAILED);
        SET_ERR_STR_FMT("Execution failed for '%s'", ctx->package->header->compilation.install);
        status = false;
    }

    /* Go back to the main directory */
    chdir("..");

end:
    ctx->state = INST_STATE_INSTALLATION;
    return status;
}

bool after_package(compile_t *ctx) {
    /* Testing if the file is there */
    if (file_exist(COMP_AFTER_SCRIPT) == false)
        goto end;

    /* Execute the before script */
    if (exec_line("sh " COMP_AFTER_SCRIPT) != 0)
    {
        SET_ERR(ERR_EXEC_FAILED);
        SET_ERR_STR_FMT("Execution failed for '%s'\n", "sh " COMP_BEFORE_SCRIPT);
        return false;
    }

end:
    ctx->state = INST_STATE_AFTER;
    return true;
}

bool install_archive(packer_t *ctx, compile_t *ptr) {
    bool        ret = false;

    /* Extract the given archive */
    if (packer_extract_archive(ctx, DEFAULT_EXTRACT_DIR) == false)
    {
        packer_free(ctx);
        return ret;
    }

    /* Init the installation process */
    ptr = package_install_init(ctx);
    if (ptr == NULL)
    {
        SET_ERR(ERR_MEMORY);
        recursive_delete(ctx->out_dir);
        packer_free(ctx);
        return ret;
    }

    if (before_package(ptr) == false)
        goto end;

    if (patch_package(ptr) == false)
        goto end;

    if (configure_package(ptr) == false)
        goto end;

    if (make_package(ptr) == false)
        goto end;

    if (install_package(ptr) == false)
        goto end;

    if (after_package(ptr) == false)
        goto end;

    ret = true;

end:
    /* Installation as failed */
    if (ret != true)
    {
        SET_ERR_STR_FMT("Installation failed at step: %s",
            install_state_to_str(ptr->state));
        SET_ERR(ERR_INSTALLATION_FAILED);
    }
    /* All good, we clean up the installation directory */
    else
    {
        recursive_delete(ctx->out_dir);
        package_install_cleanup(ptr);
    }
    return ret;
}
