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

#include <packer.h>

static packer_t *packer_init(const char *str) {
    packer_t    *ret;

    ret = malloc(sizeof(*ret));
    if (ret == NULL)
        return NULL;

    ret->json = NULL;
    ret->str = strdup(str);
    if (ret->str == NULL)
        goto cleanup;

    return ret;

cleanup:
    free(ret);
    return NULL;

}

packer_t *packer_init_dir(const char *dir) {
    packer_t    *ret;

    ret = packer_init(dir);
    if (ret == NULL)
        return NULL;
    ret->type = PACKER_TYPE_DIRECTORY;
    return ret;
}

packer_t *packer_init_archive(const char *path) {
    packer_t    *ret;

    ret = packer_init(path);
    if (ret == NULL)
        return NULL;
    ret->type = PACKER_TYPE_ARCHIVE;
    return ret;
}

void packer_free(packer_t *ptr) {
    if (ptr != NULL)
    {
        json_object_put(ptr->json);
        free(ptr->str);
        free(ptr);
    }
}

bool packer_read_dir(packer_t *ctx) {
    struct stat st;
    int         fd;
    char        *file_c = NULL;
    char        *old_pwd = getenv("PWD");

    assert(ctx != NULL);

    if (ctx->type != PACKER_TYPE_DIRECTORY)
        return false;

    if (chdir(ctx->str) == -1)
        return false;

    fd = open(PACKER_DEF_CONF_FN, O_RDONLY);
    if (fd == -1)
        goto error;

    if (fstat(fd, &st) == -1)
        goto error;

    file_c = malloc(st.st_size + 1);
    if (file_c == NULL)
        goto error;
    if (read(fd, file_c, st.st_size) == -1)
        goto error;

    file_c[st.st_size] = 0;
    ctx->json = json_tokener_parse(file_c);
    if (ctx->json == NULL)
        goto error;

    chdir(old_pwd);
    free(file_c);
    close(fd);
    return true;

error:
    if (file_c != NULL)
        free(file_c);
    if (fd != -1)
        close(fd);
    chdir(old_pwd);
    return false;
}
