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


MPX_STATIC packer_t *packer_init(const char *str) {
    packer_t    *ret;

    ret = malloc(sizeof(*ret));
    if (ret == NULL)
    {
        SET_ERR(ERR_MEMORY);
        return NULL;
    }

    ret->json = NULL;
    ret->str = strdup(str);
    ret->header = NULL;
    ret->out_dir = NULL;
    if (ret->str == NULL)
    {
        SET_ERR(ERR_MEMORY);
        goto cleanup;
    }

    return ret;

cleanup:
    free(ret);
    return NULL;
}

MPX_STATIC packer_header_package_t *packer_header_package_init(void) {
    packer_header_package_t     *ret;

    ret = malloc(sizeof(*ret));
    if (ret == NULL)
    {
        SET_ERR(ERR_MEMORY);
        return NULL;
    }

    ret->name = NULL;
    ret->version = NULL;
    ret->description = NULL;
    ret->_sbu = 0;
    ret->categ = NULL;
    ret->inst_size = 0;
    return ret;
}

MPX_STATIC void packer_header_package_free(packer_header_package_t *ptr) {
    if (ptr != NULL)
    {
        free(ptr->name);
        free(ptr->version);
        free(ptr->description);
        free(ptr->categ);
        free(ptr);
    }
}

MPX_STATIC packer_conf_opt_t *packer_conf_opt_init(const char *str, const char *value) {
    packer_conf_opt_t       *ret;

    ret = malloc(sizeof(*ret));
    if (ret == NULL)
    {
        SET_ERR(ERR_MEMORY);
        return NULL;
    }

    ret->name = NULL;
    ret->value = NULL;

    if (str != NULL)
        ret->name = strdup(str);
    if (value != NULL)
        ret->value = strdup(value);

    return ret;
}

MPX_STATIC int packer_conf_opt_free(void *magic) {
    packer_conf_opt_t *ptr = magic;
    if (ptr != NULL)
    {
        free(ptr->name);
        free(ptr->value);
    }
    return 1;
}

MPX_STATIC packer_header_comp_t *packer_header_comp_init(void) {
    packer_header_comp_t    *ret;

    ret = malloc(sizeof(*ret));
    if (ret == NULL)
    {
        SET_ERR(ERR_MEMORY);
        return NULL;
    }

    ret->configure = NULL;
    ret->make = NULL;
    ret->test = NULL;
    ret->install = NULL;
    ret->env = NULL;
    return ret;
}

MPX_STATIC void packer_header_comp_free(packer_header_comp_t *ptr) {
    if (ptr != NULL)
    {
        if (ptr->configure)
           list_free(ptr->configure, &packer_conf_opt_free);

        if (ptr->env)
           list_free(ptr->env, &packer_conf_opt_free);

        free(ptr->make);
        free(ptr->test);
        free(ptr->install);
        free(ptr);
    }
}

MPX_STATIC packer_header_deps_t *packer_header_deps_init(void) {
    packer_header_deps_t    *ret;

    ret = malloc(sizeof(*ret));
    if (ret == NULL)
    {
        SET_ERR(ERR_MEMORY);
        return NULL;
    }

    ret->list = NULL;
    return ret;
}

MPX_STATIC void packer_header_deps_free(packer_header_deps_t *ptr) {
    if (ptr != NULL)
    {
        list_free(ptr->list, NULL);
        free(ptr);
    }
}

MPX_STATIC packer_header_t *packer_header_init(void) {
    packer_header_t     *ret;

    ret = malloc(sizeof(*ret));
    if (ret == NULL)
    {
        SET_ERR(ERR_MEMORY);
        return NULL;
    }

    ret->package = NULL;
    ret->compilation = NULL;
    ret->dependencies = NULL;
    return ret;
}

MPX_STATIC void packer_header_free(packer_header_t *ptr) {
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
    {
        SET_ERR(ERR_MEMORY);
        return NULL;
    }

    ret->type = PACKER_TYPE_DIRECTORY;
    return ret;
}

packer_t *packer_init_archive(const char *path) {
    packer_t    *ret;

    ret = packer_init(path);
    if (ret == NULL)
    {
        SET_ERR(ERR_MEMORY);
        return NULL;
    }

    ret->type = PACKER_TYPE_ARCHIVE;
    return ret;
}

void packer_free(packer_t *ptr) {
    if (ptr != NULL)
    {
        json_object_put(ptr->json);
        free(ptr->str);
        packer_header_free(ptr->header);
        free(ptr->out_dir);
        free(ptr);
    }
}

MPX_STATIC bool packer_read_config_comp(packer_t *ctx, struct json_object *obj) {
    struct json_object_iterator     it, it_end;
    struct json_object              *tmp;
    const char                      *name;

    assert(ctx != NULL);

    /* Can't raise an assertion since NULL is a valid type in JSON */
    if (obj == NULL || json_object_get_type(obj) != json_type_object)
    {
        SET_ERR(ERR_BAD_JSON_TYPE);
        SET_ERR_STR("Bad global JSON type for compilation section.");
        return false;
    }

    it = json_object_iter_begin(obj);
    it_end = json_object_iter_end(obj);
    ctx->header->compilation = packer_header_comp_init();
    while (!json_object_iter_equal(&it, &it_end))
    {
        name = json_object_iter_peek_name(&it);
        tmp = json_object_iter_peek_value(&it);

        JSON_SKIP_NULL(tmp, it);
        if (strcmp(name, PACKER_CONF_COMP_CONF_TOKEN) == 0)
        {

            if (json_object_get_type(tmp) != json_type_array)
            {
                SET_ERR(ERR_BAD_JSON_TYPE);
                SET_ERR_STR_FMT("Bad JSON type for %s, expected an array", name);
                goto cleanup;
            }

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
                        {
                            SET_ERR(ERR_BAD_JSON_TYPE);
                            SET_ERR_STR("Bad JSON type for configure array, expected a string");
                            goto cleanup;
                        }

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
                {
                    SET_ERR(ERR_BAD_JSON);
                    SET_ERR_STR("Unknown JSON type in configure section");
                    goto cleanup;
                }
            }
        }
        else if (strcmp(name, PACKER_CONF_COMP_MAKE_TOKEN) == 0)
        {
            if (json_object_get_type(tmp) != json_type_string)
            {
                SET_ERR(ERR_BAD_JSON_TYPE);
                SET_ERR_STR_FMT("Bad JSON type for %s, expected a string", name);
                goto cleanup;
            }

            if (json_object_get_string_len(tmp) == 0)
                ctx->header->compilation->make = strdup(PACKER_MAKE_DEF);
            else
                ctx->header->compilation->make = strdup(json_object_get_string(tmp));
        }
        else if (strcmp(name, PACKER_CONF_COMP_TEST_TOKEN) == 0)
        {
            if (json_object_get_type(tmp) != json_type_string)
            {
                SET_ERR(ERR_BAD_JSON_TYPE);
                SET_ERR_STR_FMT("Bad JSON type for %s, expected a string", name);
                goto cleanup;
            }

            if (json_object_get_string_len(tmp) == 0)
                ctx->header->compilation->test = strdup(PACKER_TEST_DEF);
            else
                ctx->header->compilation->test = strdup(json_object_get_string(tmp));
        }
        else if (strcmp(name, PACKER_CONF_COMP_INST_TOKEN) == 0)
        {
            if (json_object_get_type(tmp) != json_type_string)
            {
                SET_ERR(ERR_BAD_JSON_TYPE);
                SET_ERR_STR_FMT("Bad JSON type for %s, expected a string", name);
                goto cleanup;
            }

            if (json_object_get_string_len(tmp) == 0)
                ctx->header->compilation->install = strdup(PACKER_INST_DEF);
            else
                ctx->header->compilation->install = strdup(json_object_get_string(tmp));
        }
        else if (strcmp(name, PACKER_CONF_COMP_ENV_TOKEN) == 0)
        {
            if (json_object_get_type(tmp) != json_type_array)
            {
                SET_ERR(ERR_BAD_JSON_TYPE);
                SET_ERR_STR_FMT("Bad JSON type for %s, expected an array", name);
                goto cleanup;
            }

            for (size_t i = 0; i < json_object_array_length(tmp); i++)
            {
                struct json_object      *array_ent, *array_tmp;
                packer_conf_opt_t       *opt = NULL;

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
                        {
                            SET_ERR(ERR_BAD_JSON_TYPE);
                            SET_ERR_STR("Bad JSON type in the env array, expected string");
                            goto cleanup;
                        }

                        opt = packer_conf_opt_init(json_object_iter_peek_name(&it_arr),
                            json_object_get_string(array_tmp));
                        list_add(ctx->header->compilation->env, opt, sizeof(*opt));
                        free(opt);
                        json_object_iter_next(&it_arr);
                    }
                }
                else if (json_object_get_type(array_ent) == json_type_string)
                {
                    opt = packer_conf_opt_init(NULL,
                        json_object_get_string(array_ent));
                    list_add(ctx->header->compilation->env, opt, sizeof(*opt));
                    free(opt);
                }
                else
                {
                    SET_ERR(ERR_BAD_JSON);
                    SET_ERR_STR("Bad JSON type in the env array, expected a string / array");
                    goto cleanup;
                }
            }
        }
        /* Wrong token */
        else
        {
            SET_ERR(ERR_BAD_JSON);
            SET_ERR_STR_FMT("Unknown JSON token: %s", name);
            goto cleanup;
        }

        json_object_iter_next(&it);

    }
    return true;

cleanup:
    packer_header_comp_free(ctx->header->compilation);
    ctx->header->compilation = NULL;
    return false;
}

MPX_STATIC bool packer_read_config_deps(packer_t *ctx, struct json_object *obj) {
    struct json_object  *tmp;
    size_t              len = 0, i = 0;

    assert(ctx != NULL);

    /* Can't raise an assertion since NULL is a valid type in JSON */
    if (obj == NULL || json_object_get_type(obj) != json_type_array)
    {
        SET_ERR(ERR_BAD_JSON_TYPE);
        return false;
    }

    ctx->header->dependencies = packer_header_deps_init();
    for (len = json_object_array_length(obj), i = 0; i < len; i++)
    {
        tmp = json_object_array_get_idx(obj, i);
        if (json_object_get_type(tmp) != json_type_string)
        {
            SET_ERR(ERR_BAD_JSON_TYPE);
            goto cleanup;
        }

        list_add(ctx->header->dependencies->list, (char *)json_object_get_string(tmp),
            json_object_get_string_len(tmp) + 1);
    }

    return true;
cleanup:
    packer_header_deps_free(ctx->header->dependencies);
    ctx->header->dependencies = NULL;
    return false;
}

MPX_STATIC bool packer_read_config_package(packer_t *ctx, struct json_object *obj) {
    struct json_object_iterator     it, it_end;
    struct json_object              *tmp;
    const char                      *name;

    assert(ctx != NULL);

    /* Can't raise an assertion since NULL is a valid type in JSON */
    if (obj == NULL || json_object_get_type(obj) != json_type_object)
    {
        SET_ERR(ERR_BAD_JSON_TYPE);
        return false;
    }

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
            {
                SET_ERR(ERR_BAD_JSON_TYPE);
                goto cleanup;
            }
            ctx->header->package->name = strdup(json_object_get_string(tmp));
        }
        else if (strcmp(name, PACKER_CONF_PACKAGE_VERSION_TOKEN) == 0)
        {
            if (json_object_get_type(tmp) != json_type_string)
            {
                SET_ERR(ERR_BAD_JSON_TYPE);
                goto cleanup;
            }
            ctx->header->package->version = strdup(json_object_get_string(tmp));
        }
        else if (strcmp(name, PACKER_CONF_PACKAGE_DESC_TOKEN) == 0)
        {
            if (json_object_get_type(tmp) != json_type_string)
            {
                SET_ERR(ERR_BAD_JSON_TYPE);
                goto cleanup;
            }
            ctx->header->package->description = strdup(json_object_get_string(tmp));
        }
        else if (strcmp(name, PACKER_CONF_PACKAGE_SBU_TOKEN) == 0)
        {
            if (json_object_get_type(tmp) != json_type_double
                && json_object_get_type(tmp) != json_type_int)
            {
                SET_ERR(ERR_BAD_JSON_TYPE);
                goto cleanup;
            }
            ctx->header->package->_sbu = json_object_get_double(tmp);
        }
        else if (strcmp(name, PACKER_CONF_PACKAGE_CATEG_TOKEN) == 0)
        {
            if (json_object_get_type(tmp) != json_type_string)
            {
                SET_ERR(ERR_BAD_JSON_TYPE);
                goto cleanup;
            }
            ctx->header->package->categ = strdup(json_object_get_string(tmp));
        }
        else if (strcmp(name, PACKER_CONF_PACKAGE_INST_SIZE_TOKEN) == 0)
        {
            if (json_object_get_type(tmp) != json_type_double
                && json_object_get_type(tmp) != json_type_int)
            {
                SET_ERR(ERR_BAD_JSON_TYPE);
                goto cleanup;
            }
            ctx->header->package->inst_size = json_object_get_double(tmp);
        }
        /* Wrong token */
        else
        {
            SET_ERR(ERR_BAD_JSON);
            goto cleanup;
        }
        json_object_iter_next(&it);
    }
    return true;

cleanup:
    packer_header_package_free(ctx->header->package);
    ctx->header->package = NULL;
    return false;
}

MPX_STATIC bool packer_read_config_file(packer_t *ctx) {
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
        {
            SET_ERR(ERR_BAD_JSON);
            goto cleanup;
        }
        json_object_iter_next(&it);
    }
    return true;

cleanup:
    packer_header_free(ctx->header);
    ctx->header = NULL;
    return false;
}

bool packer_read_dir(packer_t *ctx) {
    char        old_pwd[PATH_MAX];

    assert(ctx != NULL);

    getcwd(old_pwd, sizeof(old_pwd));

    /* If we can't get the current working directory, it's a fatal error */
    assert(old_pwd != NULL);

    if (ctx->type != PACKER_TYPE_DIRECTORY)
        goto error;

    if (chdir(ctx->str) == -1)
    {
        SET_ERR(ERR_CHDIR_FAILED);
        goto error;
    }

    ctx->json = json_object_from_file(PACKER_DEF_CONF_FN);
    if (ctx->json == NULL)
    {
        SET_ERR(ERR_BAD_JSON);
        goto error;
    }

    chdir(old_pwd);
    return packer_read_config_file(ctx);

error:
    chdir(old_pwd);
    return false;
}

MPX_STATIC void write_package_header(FILE *fd, packer_t *ctx) {
    packer_header_t     *h = ctx->header;
    mlist_t             *tmp = NULL;
    packer_conf_opt_t   *opt = NULL;
    const char          *tmp_str = NULL;
    u32_t               list_len = 0, conf_len = 0;
    double              sbu, inst_size;

    fprintf(fd, PACKER_MPX_MAGIC);
    fprintf(fd, "%s%c", h->package->name, 0);
    fprintf(fd, "%s%c", h->package->version, 0);
    fprintf(fd, "%s%c", h->package->description, 0);
    fprintf(fd, "%s%c", h->package->categ, 0);

    sbu = htonl(h->package->_sbu);
    fwrite(&sbu, sizeof(sbu), 1, fd);
    inst_size = htonl(h->package->inst_size);
    fwrite(&inst_size, sizeof(inst_size), 1, fd);

    conf_len = htonl(list_size(h->compilation->configure));
    fwrite(&conf_len, sizeof(u32_t), 1, fd);
    if (list_size(h->compilation->configure) != 0)
    {
        list_for_each(h->compilation->configure, tmp, opt) {
            if (opt->name != NULL)
                fprintf(fd, "%s:%s%c", opt->name, opt->value, 0);
            else
                fprintf(fd, "%s%c", opt->value, 0);
        }
    }
    fprintf(fd, "%s%c", STR_OR_EMPTY(h->compilation->make), 0);
    fprintf(fd, "%s%c", STR_OR_EMPTY(h->compilation->test), 0);
    fprintf(fd, "%s%c", STR_OR_EMPTY(h->compilation->install), 0);

    conf_len = htonl(list_size(h->compilation->env));
    fwrite(&conf_len, sizeof(u32_t), 1, fd);
    if (list_size(h->compilation->env) != 0)
    {
        list_for_each(h->compilation->env, tmp, opt)
        {
            if (opt->name != NULL)
                fprintf(fd, "%s:%s%c", opt->value, opt->name, 0);
            else
                fprintf(fd, "%s%c", opt->value, 0);
        }
    }

    list_len = htonl(list_size(h->dependencies->list));
    fwrite(&list_len, sizeof(u32_t), 1, fd);

    list_for_each(h->dependencies->list, tmp, tmp_str) {
        fprintf(fd, "%s%c", tmp_str, '\0');
    }
}

MPX_STATIC bool write_packer_sources(FILE *fd, packer_t *ctx, const char *dir_name) {
    mlist_t         *files_list = NULL;
    mlist_t         *dirs = NULL;
    mlist_t         *tmp = NULL, *tmp2 = NULL;
    char            old_pwd[PATH_MAX];
    char            *dir = NULL;
    packer_file_t   *file;

    assert(fd != NULL && ctx != NULL);

    getcwd(old_pwd, sizeof(old_pwd));

    /* If we can't get the current working directory, it's a fatal error */
    assert(old_pwd != NULL);

    chdir(ctx->str);
    list_add(dirs, (void *)dir_name, strlen(dir_name) + 1);

    list_for_each(dirs, tmp, dir) {
        if (read_files_from_dir(dir, &files_list, &dirs) == false)
            goto error;

        if (list_size(files_list) == 0)
            continue;

        list_for_each(files_list, tmp2, file) {
            fprintf(fd, "%s%c", file->fn, 0);
            fwrite(&file->mode, sizeof(file->mode), 1, fd);
            fwrite(&file->compressed_size, sizeof(file->compressed_size), 1, fd);
            fwrite(&file->file_size, sizeof(file->file_size), 1, fd);
            if (file->file_size != 0)
            {
                fwrite(&file->sum, crypto_hash_sha256_BYTES, 1, fd);
                fwrite(file->content, file->compressed_size, 1, fd);
            }
        }
        list_free(files_list, packer_file_free);
        files_list = NULL;
    }

    list_free(dirs, NULL);
    chdir(old_pwd);
    return true;

error:
    list_free(files_list, packer_file_free);
    list_free(dirs, NULL);
    chdir(old_pwd);
    return false;
}

bool packer_create_archive(packer_t *ctx, const char *archive_path) {
    FILE *fd;

    assert(archive_path == NULL || ctx != NULL);
    if (ctx->type != PACKER_TYPE_DIRECTORY)
    {
        SET_ERR(ERR_BAD_ARCHIVE_TYPE);
        return false;
    }

    fd = fopen(archive_path, "w+");
    if (fd == NULL)
    {
        SET_ERR(ERR_OPEN);
        return false;
    }

    write_package_header(fd, ctx);
    if (write_packer_sources(fd, ctx, PACKER_SRC_DIR) == false)
        goto error;

    if (write_packer_sources(fd, ctx, PACKER_PATCH_DIR) == false)
        goto error;

    if (write_packer_sources(fd, ctx, PACKER_SCRIPT_DIR) == false)
        goto error;

    fclose(fd);
    return true;

error:
    fclose(fd);
    return false;
}

MPX_STATIC int read_package_header_package(const char *file, packer_t *ctx)
{
    int                         ret = 0;
    packer_header_package_t     *pkg = NULL;
    double              sbu, inst_size;

    pkg = packer_header_package_init();
    if (pkg == NULL)
        return ret;

    pkg->name = strdup(file + ret);
    if (pkg->name == NULL)
    {
        SET_ERR(ERR_MEMORY);
        goto cleanup;
    }

    ret += strlen(pkg->name) + 1;

    pkg->version = strdup(file + ret);
    if (pkg->version == NULL)
        goto cleanup;

    ret += strlen(pkg->version) + 1;

    pkg->description = strdup(file + ret);
    if (pkg->description == NULL)
        goto cleanup;

    ret += strlen(pkg->description) + 1;

    pkg->categ = strdup(file + ret);
    if (pkg->categ == NULL)
        goto cleanup;

    ret += strlen(pkg->categ) + 1;

    memcpy(&sbu, file + ret, sizeof(sbu));
    pkg->_sbu = ntohl(sbu);
    ret += sizeof(sbu);

    memcpy(&inst_size, file + ret, sizeof(inst_size));
    pkg->inst_size = ntohl(inst_size);
    ret += sizeof(inst_size);

    ctx->header->package = pkg;
    return ret;

cleanup:
    SET_ERR(ERR_MEMORY);
    packer_header_package_free(pkg);
    return 0;
}

MPX_STATIC bool read_conf_opt(char *file, mlist_t **list, int *ret) {
    u32_t               size;
    packer_conf_opt_t   *opt = NULL;
    char                *tmp = NULL, *ptr = NULL;

    memcpy(&size, file + *ret, sizeof(size));
    size = ntohl(size);
    *ret += sizeof(size);

    for (u32_t i = 0; i < size; i++)
    {
        opt = malloc(sizeof(*opt));
        if (opt == NULL)
        {
            SET_ERR(ERR_MEMORY);
            goto cleanup;
        }

        tmp = file + *ret;
        *ret += strlen(tmp) + 1;

        ptr = strchr(tmp, ':');
        if (ptr == NULL)
        {
            opt->value = strdup(tmp);
            if (opt->value == NULL)
            {
                SET_ERR(ERR_MEMORY);
                free(opt);
                goto cleanup;
            }
            opt->name = NULL;
        }
        else
        {
            *ptr = 0;
            opt->name = strdup(tmp);
            if (opt->name == NULL)
            {
                SET_ERR(ERR_MEMORY);
                free(opt);
                goto cleanup;
            }

            opt->value = strdup(ptr + 1);
            if (opt->value == NULL)
            {
                free(opt->name);
                free(opt);
                SET_ERR(ERR_MEMORY);
                goto cleanup;
            }
        }
        list_add(*(list), opt, sizeof(*opt));
        free(opt);
    }

    return true;

cleanup:
    list_free(*(list), packer_conf_opt_free);
    *list = NULL;
    return false;
}

MPX_STATIC int read_package_header_compilation(char *file, packer_t *ctx)
{
    packer_header_comp_t        *comp = NULL;
    int                         ret = 0;

    comp = packer_header_comp_init();
    if (comp == NULL)
        return 0;

    if (read_conf_opt(file, &comp->configure, &ret) != true)
        goto cleanup;

    comp->make = strdup(file + ret);
    if (comp->make == NULL)
    {
        SET_ERR(ERR_MEMORY);
        goto cleanup;
    }

    ret += strlen(comp->make) + 1;

    comp->test = strdup(file + ret);
    if (comp->test == NULL)
    {
        SET_ERR(ERR_MEMORY);
        goto cleanup;
    }
    ret += strlen(comp->test) + 1;

    comp->install = strdup(file + ret);
    if (comp->install == NULL)
    {
        SET_ERR(ERR_MEMORY);
        goto cleanup;
    }
    ret += strlen(comp->install) + 1;

    if (read_conf_opt(file, &comp->env, &ret) != true)
        goto cleanup;

    ctx->header->compilation = comp;
    return ret;

cleanup:
    packer_header_comp_free(comp);
    return 0;
}

MPX_STATIC int read_package_header_dependencies(const char *file, packer_t *ctx) {
    packer_header_deps_t        *deps = NULL;
    int                         ret = 0;
    u32_t                       list_len = 0;
    char                        *tmp = NULL;

    deps = packer_header_deps_init();
    if (deps == NULL)
        return 0;
    memcpy(&list_len, file, sizeof(u32_t));
    list_len = ntohl(list_len);
    ret += sizeof(u32_t);

    for (u32_t i = 0; i < list_len; i++)
    {
        tmp = strdup(file + ret);
        if (tmp == NULL)
        {
            SET_ERR(ERR_MEMORY);
            goto cleanup;
        }
        ret += strlen(tmp) + 1;
        list_add(deps->list, tmp, strlen(tmp) + 1);
        free(tmp);
    }

    ctx->header->dependencies = deps;
    return ret;

cleanup:
    packer_header_deps_free(deps);
    return 0;
}

MPX_STATIC bool read_package_header(char *file_content, packer_t *ctx, int *s_ret)
{
    int  ret = 0, tmp;

    if (file_content == NULL)
    {
        SET_ERR(ERR_BAD_PTR);
        return false;
    }

    if (strncmp(file_content, PACKER_MPX_MAGIC, sizeof(PACKER_MPX_MAGIC) - 1) != 0)
    {
        SET_ERR(ERR_NOT_A_PACKAGE);
        goto cleanup;
    }

    ret += sizeof(PACKER_MPX_MAGIC) - 1;

    ctx->header = packer_header_init();
    if (ctx->header == NULL)
        goto cleanup;

    tmp = read_package_header_package(file_content + ret, ctx);
    if (tmp == 0)
        goto cleanup;
    ret += tmp;

    tmp = read_package_header_compilation(file_content + ret, ctx);
    if (tmp == 0)
        goto cleanup;
    ret += tmp;

    tmp = read_package_header_dependencies(file_content + ret, ctx);
    if (tmp == 0)
        goto cleanup;

    ret += tmp;

    *s_ret = ret;
    return true;

cleanup:
    packer_header_free(ctx->header);
    ctx->header = NULL;
    return false;
}

bool packer_read_archive_header(packer_t *ctx) {
    char    *archive = NULL;
    bool    ret;
    int     cur = 0;

    if (ctx->type != PACKER_TYPE_ARCHIVE)
        return false;

    archive = mpm_read_file_from_fn(ctx->str);
    if (archive == NULL)
    {
        SET_ERR(ERR_MEMORY);
        return false;
    }

    ret = read_package_header(archive, ctx, &cur);
    free(archive);
    return ret;
}

bool packer_extract_archive(packer_t *ctx, const char *dir) {
    int                 fd, t_ctr;
    bool                ret = false;
    char                *archive = NULL;
    off_t               size, ctr;
    char                old_pwd[PATH_MAX];

    getcwd(old_pwd, sizeof(old_pwd));

    if (ctx->type != PACKER_TYPE_ARCHIVE)
    {
        SET_ERR(ERR_BAD_ARCHIVE_TYPE);
        SET_ERR_STR("Not an MPX archive");
        return false;
    }

    fd = open(ctx->str, O_RDONLY);
    if (fd == -1)
    {
        SET_ERR(ERR_OPEN);
        SET_ERR_STR_FMT("Can't open archive '%s'", ctx->str);
        return false;
    }

    archive = mpm_read_file_from_fd(fd);
    size = mpm_get_file_size_from_fd(fd);

    if (!read_package_header(archive, ctx, &t_ctr))
        goto cleanup;

    ctr = t_ctr;

    DIR *p_dir = opendir(dir);
    if (p_dir == NULL)
        mkdir(dir, S_IRWXU | S_IRWXG | S_IRWXO);
    else
        closedir(p_dir);

    if (chdir(dir) == -1)
    {
        SET_ERR(ERR_CHDIR_FAILED);
        SET_ERR_STR_FMT("Cannot go in the directory '%s'\n", dir);
        goto cleanup;
    }

    asprintf(&ctx->out_dir, "%s/%s-%s/", dir,
        ctx->header->package->name, ctx->header->package->version);

    if (mkdir(ctx->out_dir, S_IRWXU | S_IRWXG | S_IRWXO) == -1)
    {
        SET_ERR(ERR_MKDIR_FAILED);
        SET_ERR_STR_FMT("Cannot create directory '%s'\n", ctx->out_dir);
        goto cleanup;
    }

    chdir(ctx->out_dir);

    while (size > ctr)
    {
        if (packer_file_from_binary_to_disk(archive, &ctr) == false)
            goto cleanup;
    }

    ret = true;

cleanup:
    chdir(old_pwd);
    close(fd);
    free(archive);
    return ret;
}
