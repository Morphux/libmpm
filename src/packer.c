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

packer_t *packer_init(const char *dir) {
    packer_t    *ret;

    ret = malloc(sizeof(*ret));
    if (ret == NULL)
        return NULL;

    ret->dir = strdup(dir);
    if (ret->dir == NULL)
        goto cleanup;
    return ret;

cleanup:
    free(ret);
    return NULL;
}

void packer_free(packer_t *ptr) {
    if (ptr != NULL)
    {
        free(ptr->dir);
        free(ptr);
    }
}

bool packer_read(packer_t *ctx) {
    struct stat st;
    int         fd;
    char        *fn = strcat(ctx->dir, PACKER_DEF_CONF_FN);
    char        *file_c = NULL;

    assert(fn != NULL);
    fd = open(fn, O_RDONLY);
    if (fd == -1)
        goto error;
    if (fstat(fd, &st) == -1)
        goto error;

    file_c = malloc(st.st_size);
    if (file_c == NULL)
        goto error;
    if (read(fd, file_c, st.st_size) == -1)
        goto error;
    ctx->json = json_tokener_parse(file_c);

    if (ctx->json == NULL)
        goto error;

    free(fn);
    free(file_c);
    close(fd);
    return true;

error:
    free(fn);
    if (file_c != NULL)
        free(file_c);
    close(fd);
    return false;
}
