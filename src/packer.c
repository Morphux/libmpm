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

static packer_header_package_t *packer_header_package_init(void) {
    packer_header_package_t     *ret;

    ret = malloc(sizeof(*ret));
    if (ret == NULL)
        return NULL;

    ret->name = NULL;
    ret->version = NULL;
    ret->description = NULL;
    return ret;
}

static void packer_header_package_free(packer_header_package_t *ptr) {
    if (ptr != NULL)
    {
        free(ptr->name);
        free(ptr->version);
        free(ptr->description);
        free(ptr);
    }
}

static packer_header_t *packer_header_init(void) {
    packer_header_t     *ret;

    ret = malloc(sizeof(*ret));
    if (ret == NULL)
        return NULL;

    ret->package = NULL;
    return ret;
}

static void packer_header_free(packer_header_t *ptr) {
    if (ptr != NULL)
    {
        packer_header_package_free(ptr->package);
        free(ptr);
    }
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

static bool packer_read_config_package(packer_t *ctx, struct json_object *obj)
{
    struct json_object_iterator     it, it_end;
    struct json_object              *tmp;
    const char                      *name;

    assert(ctx != NULL);

    /* Can't raise an assertion since NULL is a valid type in JSON */
    if (obj == NULL)
        return false;

    it = json_object_iter_begin(obj);
    it_end = json_object_iter_end(obj);
    ctx->header->package = packer_header_package_init();

    while (!json_object_iter_equal(&it, &it_end))
    {
        name = json_object_iter_peek_name(&it);
        if (strcmp(name, PACKER_CONF_PACKAGE_NAME_TOKEN) == 0)
        {
            tmp = json_object_iter_peek_value(&it);
            if (json_object_get_type(tmp) != json_type_string)
                goto cleanup;
            ctx->header->package->name = strdup(json_object_get_string(tmp));
        }
        else if (strcmp(name, PACKER_CONF_PACKAGE_VERSION_TOKEN) == 0)
        {
            tmp = json_object_iter_peek_value(&it);
            if (json_object_get_type(tmp) != json_type_string)
                goto cleanup;
            ctx->header->package->version = strdup(json_object_get_string(tmp));
        }
        else if (strcmp(name, PACKER_CONF_PACKAGE_DESC_TOKEN) == 0)
        {
            tmp = json_object_iter_peek_value(&it);
            if (json_object_get_type(tmp) != json_type_string)
                goto cleanup;
            ctx->header->package->description = strdup(json_object_get_string(tmp));
        }
        /* Wrong token */
        else
            goto cleanup;
        json_object_iter_next(&it);
    }
    return true;

cleanup:
    packer_header_package_free(ctx->header->package);
    return false;
}

static bool packer_read_config_file(packer_t *ctx) {
    struct json_object_iterator     it, it_end;
    const char                      *name;

    assert(ctx != NULL);
    it = json_object_iter_begin(ctx->json);
    it_end = json_object_iter_end(ctx->json);
    ctx->header = packer_header_init();

    while (!json_object_iter_equal(&it, &it_end))
    {
        name = json_object_iter_peek_name(&it);
        if (strcmp(name, PACKER_CONF_PACKAGE_TOKEN) == 0)
            packer_read_config_package(ctx, json_object_iter_peek_value(&it));
        else if (strcmp(name, "compilation"))
            ;
        else if (strcmp(name, "dependencies"))
            ;
        /* Wrong token */
        else
            goto cleanup;
        json_object_iter_next(&it);
    }
    return true;

cleanup:
    packer_header_free(ctx->header);
    return false;
}

bool packer_read_dir(packer_t *ctx) {
    char        *old_pwd = getenv("PWD");

    assert(ctx != NULL);

    if (ctx->type != PACKER_TYPE_DIRECTORY)
        return false;
    if (chdir(ctx->str) == -1)
        return false;

    ctx->json = json_object_from_file(PACKER_DEF_CONF_FN);
    if (ctx->json == NULL)
        goto error;

    chdir(old_pwd);
    return packer_read_config_file(ctx);

error:
    chdir(old_pwd);
    return false;
}

