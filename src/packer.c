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
    ret->header = NULL;
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

static packer_conf_opt_t *packer_conf_opt_init(const char *str, const char *value) {
    packer_conf_opt_t       *ret;

    ret = malloc(sizeof(*ret));
    if (ret == NULL)
        return NULL;

    ret->name = NULL;
    ret->value = NULL;

    if (str != NULL)
        ret->name = strdup(str);
    if (value != NULL)
        ret->value = strdup(value);
    return ret;
}

static int packer_conf_opt_free(void *magic) {
    packer_conf_opt_t *ptr = magic;
    if (ptr != NULL)
    {
        free(ptr->name);
        free(ptr->value);
    }
    return 1;
}

static packer_header_comp_t *packer_header_comp_init(void) {
    packer_header_comp_t    *ret;

    ret = malloc(sizeof(*ret));
    if (ret == NULL)
        return NULL;

    ret->configure = NULL;
    ret->make = NULL;
    ret->test = NULL;
    ret->install = NULL;
    return ret;
}

static void packer_header_comp_free(packer_header_comp_t *ptr) {
    if (ptr != NULL)
    {
        if (ptr->configure)
           list_free(ptr->configure, &packer_conf_opt_free);
        free(ptr->make);
        free(ptr->test);
        free(ptr->install);
        free(ptr);
    }
}

static packer_header_deps_t *packer_header_deps_init(void) {
    packer_header_deps_t    *ret;

    ret = malloc(sizeof(*ret));
    if (ret == NULL)
        return NULL;

    ret->list = NULL;
    return ret;
}

static void packer_header_deps_free(packer_header_deps_t *ptr) {
    if (ptr != NULL)
    {
        list_free(ptr->list, NULL);
        free(ptr);
    }
}

static packer_header_t *packer_header_init(void) {
    packer_header_t     *ret;

    ret = malloc(sizeof(*ret));
    if (ret == NULL)
        return NULL;

    ret->package = NULL;
    ret->compilation = NULL;
    ret->dependencies = NULL;
    return ret;
}

static void packer_header_free(packer_header_t *ptr) {
    if (ptr != NULL)
    {
        packer_header_package_free(ptr->package);
        packer_header_comp_free(ptr->compilation);
        packer_header_deps_free(ptr->dependencies);
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
        packer_header_free(ptr->header);
        free(ptr);
    }
}

static bool packer_read_config_comp(packer_t *ctx, struct json_object *obj) {
    struct json_object_iterator     it, it_end;
    struct json_object              *tmp;
    const char                      *name;

    assert(ctx != NULL);

    /* Can't raise an assertion since NULL is a valid type in JSON */
    if (obj == NULL || json_object_get_type(obj) != json_type_object)
        return false;

    it = json_object_iter_begin(obj);
    it_end = json_object_iter_end(obj);
    ctx->header->compilation = packer_header_comp_init();
    while (!json_object_iter_equal(&it, &it_end))
    {
        name = json_object_iter_peek_name(&it);
        tmp = json_object_iter_peek_value(&it);
        if (strcmp(name, PACKER_CONF_COMP_CONF_TOKEN) == 0)
        {
            if (json_object_get_type(tmp) != json_type_array)
                goto cleanup;
            for (size_t i = 0; i < json_object_array_length(tmp); i++)
            {
                struct json_object  *array_ent, *array_tmp;
                packer_conf_opt_t   *opt = NULL;

                array_ent = json_object_array_get_idx(tmp, i);
                if (json_object_get_type(array_ent) == json_type_object)
                {
                    struct json_object_iterator     it_arr, it_arr_end;

                    it_arr = json_object_iter_begin(array_ent);
                    it_arr_end = json_object_iter_end(array_ent);
                    while (!json_object_iter_equal(&it_arr, &it_arr_end))
                    {
                        array_tmp = json_object_iter_peek_value(&it_arr);
                        if (json_object_get_type(array_tmp) != json_type_string)
                            goto cleanup;
                        opt = packer_conf_opt_init(json_object_iter_peek_name(&it_arr),
                            json_object_get_string(array_tmp));
                        list_add(ctx->header->compilation->configure, opt, sizeof(*opt));
                        free(opt);
                        json_object_iter_next(&it_arr);
                    }
                }
                else if (json_object_get_type(array_ent) == json_type_string)
                {
                    opt = packer_conf_opt_init(NULL,
                        json_object_get_string(array_ent));
                    list_add(ctx->header->compilation->configure, opt, sizeof(*opt));
                    free(opt);
                }
                else
                    goto cleanup;
            }
        }
        else if (strcmp(name, PACKER_CONF_COMP_MAKE_TOKEN) == 0)
        {
            if (json_object_get_type(tmp) != json_type_string)
                goto cleanup;
            ctx->header->compilation->make = strdup(json_object_get_string(tmp));
        }
        else if (strcmp(name, PACKER_CONF_COMP_TEST_TOKEN) == 0)
        {
            if (json_object_get_type(tmp) != json_type_string)
                goto cleanup;
            ctx->header->compilation->test = strdup(json_object_get_string(tmp));
        }
        else if (strcmp(name, PACKER_CONF_COMP_INST_TOKEN) == 0)
        {
            if (json_object_get_type(tmp) != json_type_string)
                goto cleanup;
            ctx->header->compilation->install = strdup(json_object_get_string(tmp));
        }
        /* Wrong token */
        else
            goto cleanup;
        json_object_iter_next(&it);

    }
    return true;

cleanup:
    packer_header_comp_free(ctx->header->compilation);
    ctx->header->compilation = NULL;
    return false;
}

static bool packer_read_config_deps(packer_t *ctx, struct json_object *obj) {
    struct json_object  *tmp;
    size_t              len = 0, i = 0;

    assert(ctx != NULL);

    /* Can't raise an assertion since NULL is a valid type in JSON */
    if (obj == NULL || json_object_get_type(obj) != json_type_array)
        return false;

    ctx->header->dependencies = packer_header_deps_init();
    for (len = json_object_array_length(obj), i = 0; i < len; i++)
    {
        tmp = json_object_array_get_idx(obj, i);
        if (json_object_get_type(tmp) != json_type_string)
            goto cleanup;
        list_add(ctx->header->dependencies->list, (char *)json_object_get_string(tmp),
            json_object_get_string_len(tmp));
    }

    return true;
cleanup:
    packer_header_deps_free(ctx->header->dependencies);
    ctx->header->dependencies = NULL;
    return false;
}

static bool packer_read_config_package(packer_t *ctx, struct json_object *obj) {
    struct json_object_iterator     it, it_end;
    struct json_object              *tmp;
    const char                      *name;

    assert(ctx != NULL);

    /* Can't raise an assertion since NULL is a valid type in JSON */
    if (obj == NULL || json_object_get_type(obj) != json_type_object)
        return false;

    it = json_object_iter_begin(obj);
    it_end = json_object_iter_end(obj);
    ctx->header->package = packer_header_package_init();

    while (!json_object_iter_equal(&it, &it_end))
    {
        name = json_object_iter_peek_name(&it);
        tmp = json_object_iter_peek_value(&it);
        if (strcmp(name, PACKER_CONF_PACKAGE_NAME_TOKEN) == 0)
        {
            if (json_object_get_type(tmp) != json_type_string)
                goto cleanup;
            ctx->header->package->name = strdup(json_object_get_string(tmp));
        }
        else if (strcmp(name, PACKER_CONF_PACKAGE_VERSION_TOKEN) == 0)
        {
            if (json_object_get_type(tmp) != json_type_string)
                goto cleanup;
            ctx->header->package->version = strdup(json_object_get_string(tmp));
        }
        else if (strcmp(name, PACKER_CONF_PACKAGE_DESC_TOKEN) == 0)
        {
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
    ctx->header->package = NULL;
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
        {
            if (!packer_read_config_package(ctx, json_object_iter_peek_value(&it)))
                goto cleanup;
        }
        else if (strcmp(name, PACKER_CONF_COMP_TOKEN) == 0)
        {
            if (!packer_read_config_comp(ctx, json_object_iter_peek_value(&it)))
                goto cleanup;
        }
        else if (strcmp(name, PACKER_CONF_DEPS_TOKEN) == 0)
        {
            if (!packer_read_config_deps(ctx, json_object_iter_peek_value(&it)))
                goto cleanup;
        }
        /* Wrong token */
        else
            goto cleanup;
        json_object_iter_next(&it);
    }
    return true;

cleanup:
    packer_header_free(ctx->header);
    ctx->header = NULL;
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

