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

compile_t *package_install_init(packer_t *ctx) {
    compile_t           *ret = NULL;
    packer_conf_opt_t   *opt = NULL;
    mlist_t             *tmp = NULL;

    ret = malloc(sizeof(*ret));
    if (ret == NULL)
    {
        set_mpm_error(ERR_MEMORY);
        return NULL;
    }

    ret->package = ctx;
    ret->state = INST_STATE_NONE;

    getcwd(ret->old_pwd, sizeof(ret->old_pwd));
    if (chdir(ctx->out_dir) != 0)
    {
        free(ret);
        set_mpm_error(ERR_MEMORY);
        return NULL;
    }
    if (ctx->header->compilation->env == NULL)
        return ret;

    list_for_each(ctx->header->compilation->env, tmp, opt)
    {
        if (opt->name != NULL)
            setenv(opt->value, opt->name, 1);
        else
            setenv(opt->value, "", 1);
    }

    return ret;
}

bool package_install_cleanup(compile_t *ctx) {
    if (chdir(ctx->old_pwd) != 0)
    {
        set_mpm_error(ERR_CHDIR_FAILED);
        return false;
    }

    packer_free(ctx->package);
    free(ctx);
    return true;
}

bool before_package(compile_t *ctx) {
    if (file_exist(COMP_BEFORE_SCRIPT) == false)
        goto end;

    if (exec_line("sh " COMP_BEFORE_SCRIPT) != 0)
    {
        set_mpm_error(ERR_EXEC_FAILED);
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

    if (chdir(PACKER_SRC_DIR) == -1)
    {
        set_mpm_error(ERR_CHDIR_FAILED);
        goto end;
    }

    while ((dinfo = readdir(dir)))
    {
        /* Skip .* files */
        if (strlen(dinfo->d_name) > 0 && dinfo->d_name[0] == '.')
            continue ;

        if (dinfo->d_type != DT_DIR)
        {
            asprintf(&cmd, "%s ../%s%s", PATCH_CMD, PACKER_PATCH_DIR, dinfo->d_name);
            if (exec_line(cmd) != 0)
            {
                set_mpm_error(ERR_EXEC_FAILED);
                free(cmd);
                goto end;
            }
            free(cmd);
        }
    }

    if (chdir("..") == -1)
    {
        set_mpm_error(ERR_CHDIR_FAILED);
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
    packer_conf_opt_t   *opt;
    char                *arg = NULL;
    bool                ret = false;

    /* Nothing to configure, we're good */
    if (ctx->package->header->compilation->configure == NULL)
    {
        ret = true;
        goto end;
    }

    if (chdir(PACKER_SRC_DIR) == -1)
    {
        set_mpm_error(ERR_CHDIR_FAILED);
        goto end;
    }

    list_add(cmd, CONFIGURE_CMD, sizeof(CONFIGURE_CMD) + 1);

    list_for_each(ctx->package->header->compilation->configure, tmp, opt) {
        if (opt->name != NULL)
            asprintf(&arg, "--%s=%s", opt->name, opt->value);
        else
            asprintf(&arg, "--%s", opt->value);
        list_add(cmd, arg, strlen(arg) + 1);
        free(arg);
    }

    if (exec_list(cmd) == 0)
        ret = true;
    else
        set_mpm_error(ERR_EXEC_FAILED);

    list_free(cmd, NULL);

    chdir("..");

end:
    ctx->state = INST_STATE_CONFIGURATION;
    return ret;
}

bool make_package(compile_t *ctx) {
    bool status = true;

    /* Nothing to compile, we're good */
    if (STR_NULL_OR_EMPTY(ctx->package->header->compilation->make))
        goto end;

    if (chdir(PACKER_SRC_DIR))
    {
        set_mpm_error(ERR_CHDIR_FAILED);
        return false;
    }

    if (exec_line(ctx->package->header->compilation->make) != 0)
    {
        set_mpm_error(ERR_EXEC_FAILED);
        status = false;
    }

    chdir("..");

end:
    ctx->state = INST_STATE_COMPILATION;
    return status;
}

bool install_package(compile_t *ctx) {
    bool status = true;

    /* Nothing to install, we're good */
    if (STR_NULL_OR_EMPTY(ctx->package->header->compilation->install))
        goto end;

    if (chdir(PACKER_SRC_DIR))
    {
        set_mpm_error(ERR_CHDIR_FAILED);
        return false;
    }

    if (exec_line(ctx->package->header->compilation->install) != 0)
    {
        set_mpm_error(ERR_EXEC_FAILED);
        status = false;
    }

    chdir("..");

end:
    ctx->state = INST_STATE_INSTALLATION;
    return status;
}

bool after_package(compile_t *ctx) {
    if (file_exist(COMP_AFTER_SCRIPT) == false)
        goto end;

    if (exec_line("sh " COMP_AFTER_SCRIPT) != 0)
    {
        set_mpm_error(ERR_EXEC_FAILED);
        return false;
    }

end:
    ctx->state = INST_STATE_AFTER;
    return true;
}

bool install_archive(packer_t *ctx) {
    bool        ret = false;
    compile_t   *ptr = NULL;

    if (packer_extract_archive(ctx, DEFAULT_EXTRACT_DIR) == false)
    {
        packer_free(ctx);
        set_mpm_error(ERR_ARCHIVE_EXTRACT);
        return ret;
    }

    ptr = package_install_init(ctx);
    if (ptr == NULL)
    {
        set_mpm_error(ERR_MEMORY);
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
    if (ret != true)
        set_mpm_error(ERR_INSTALLATION_FAILED);
    recursive_delete(ctx->out_dir);
    package_install_cleanup(ptr);
    return ret;
}
