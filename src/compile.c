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
    compile_t   *ret = NULL;

    ret = malloc(sizeof(*ret));
    if (ret == NULL)
        return NULL;

    ret->package = ctx;
    ret->state = INST_STATE_NONE;

    getcwd(ret->old_pwd, sizeof(ret->old_pwd));
    if (chdir(ctx->out_dir) != 0)
    {
        free(ret);
        return NULL;
    }
    return ret;
}

bool package_install_cleanup(compile_t *ctx) {
    if (chdir(ctx->old_pwd) != 0)
        return false;

    packer_free(ctx->package);
    free(ctx);
    return true;
}

bool before_package(compile_t *ctx) {
    if (file_exist(COMP_BEFORE_SCRIPT) == false)
        goto end;

    if (exec_line("sh " COMP_BEFORE_SCRIPT) != 0)
        return false;

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
        goto end;

    while ((dinfo = readdir(dir)))
    {
        /* Skip .* files */
        if (strlen(dinfo->d_name) > 0 && dinfo->d_name[0] == '.')
            continue ;

        if (dinfo->d_type != DT_DIR)
        {
            asprintf(&cmd, "%s ../%s%s", PATCH_CMD, PACKER_PATCH_DIR, dinfo->d_name);
            exec_line(cmd);
            free(cmd);
        }
    }

    if (chdir("..") == -1)
        goto end;

    status = true;
end:
    closedir(dir);
    ctx->state = INST_STATE_PATCHING;
    return status;
}

bool configure_package(compile_t *ctx) {
    /* Nothing to configure, we're good */
    if (ctx->package->header->compilation->configure == NULL)
        goto end;

end:
    ctx->state = INST_STATE_CONFIGURATION;
    return true;
}

bool make_package(compile_t *ctx) {
    bool status = true;

    /* Nothing to compile, we're good */
    if (STR_NULL_OR_EMPTY(ctx->package->header->compilation->make))
        goto end;

    if (chdir(PACKER_SRC_DIR))
        return false;

    if (exec_line(ctx->package->header->compilation->make) != 0)
        status = false;

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
        return false;

    if (exec_line(ctx->package->header->compilation->install) != 0)
        status = false;

    chdir("..");

end:
    ctx->state = INST_STATE_INSTALLATION;
    return status;
}

bool after_package(compile_t *ctx) {
    if (file_exist(COMP_AFTER_SCRIPT) == false)
        goto end;

    if (exec_line("sh " COMP_AFTER_SCRIPT) != 0)
        return false;

end:
    ctx->state = INST_STATE_AFTER;
    return true;
}
