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
    ret->header = NULL;
    ret->out_dir = NULL;

    ret->str = strdup(str);
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

MPX_STATIC packer_header_t *packer_header_init(void) {
    packer_header_t     *ret;

    ret = malloc(sizeof(*ret));
    if (ret == NULL)
    {
        SET_ERR(ERR_MEMORY);
        return NULL;
    }

    packer_header_package_init(ret);
    packer_header_comp_init(ret);
    packer_header_deps_init(ret);

    return ret;
}

MPX_STATIC void packer_header_free(packer_header_t *ptr) {
    if (ptr != NULL)
    {
        packer_header_package_free(ptr);
        packer_header_comp_free(ptr);
        packer_header_deps_free(ptr);
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

    /* Get the first and last element */
    it = json_object_iter_begin(obj);
    it_end = json_object_iter_end(obj);

    /* Iterating for each member */
    while (!json_object_iter_equal(&it, &it_end))
    {
        name = json_object_iter_peek_name(&it);
        tmp = json_object_iter_peek_value(&it);

        /* Skip NULL (JSON null) values */
        JSON_SKIP_NULL(tmp, it);

        if (strcmp(name, PACKER_CONF_COMP_CONF_TOKEN) == 0)
        {

            /* Element type is unexpected */
            if (json_object_get_type(tmp) != json_type_array)
            {
                SET_ERR(ERR_BAD_JSON_TYPE);
                SET_ERR_STR_FMT("Bad JSON type for %s, expected an array", name);
                goto cleanup;
            }

            /* Iteraite over each element of the array */
            for (size_t i = 0; i < json_object_array_length(tmp); i++)
            {
                struct json_object      *array_ent, *array_tmp;
                vector_string_t         *opt = NULL;

                /* Get ID of the current member */
                array_ent = json_object_array_get_idx(tmp, i);

                /**
                 * If it's an array, we need to read each member
                 */
                if (json_object_get_type(array_ent) == json_type_object)
                {
                    struct json_object_iterator     it_arr, it_arr_end;

                    /* Get the first and last member */
                    it_arr = json_object_iter_begin(array_ent);
                    it_arr_end = json_object_iter_end(array_ent);
                    while (!json_object_iter_equal(&it_arr, &it_arr_end))
                    {
                        /* Get the value */
                        array_tmp = json_object_iter_peek_value(&it_arr);

                        /* Member type is unexpected */
                        if (json_object_get_type(array_tmp) != json_type_string)
                        {
                            SET_ERR(ERR_BAD_JSON_TYPE);
                            SET_ERR_STR("Bad JSON type for configure array, expected a string");
                            goto cleanup;
                        }

                        /* Get the value */
                        opt = vector_string_init(json_object_iter_peek_name(&it_arr),
                            json_object_get_string(array_tmp));

                        /* Add to the context */
                        list_add(ctx->header->compilation.configure, opt, sizeof(*opt));
                        free(opt);

                        /* Get the next member */
                        json_object_iter_next(&it_arr);
                    }
                }
                else if (json_object_get_type(array_ent) == json_type_string)
                {
                    /* Get the value */
                    opt = vector_string_init(NULL, json_object_get_string(array_ent));

                    /* Add to the context */
                    list_add(ctx->header->compilation.configure, opt, sizeof(*opt));
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
            /* Member type is unexpected */
            if (json_object_get_type(tmp) != json_type_string)
            {
                SET_ERR(ERR_BAD_JSON_TYPE);
                SET_ERR_STR_FMT("Bad JSON type for %s, expected a string", name);
                goto cleanup;
            }

            /* If the string is empty, set the default Make command */
            if (json_object_get_string_len(tmp) == 0)
                ctx->header->compilation.make = strdup(PACKER_MAKE_DEF);
            else
                ctx->header->compilation.make = strdup(json_object_get_string(tmp));
        }
        else if (strcmp(name, PACKER_CONF_COMP_TEST_TOKEN) == 0)
        {
            /* Member type is unexpected */
            if (json_object_get_type(tmp) != json_type_string)
            {
                SET_ERR(ERR_BAD_JSON_TYPE);
                SET_ERR_STR_FMT("Bad JSON type for %s, expected a string", name);
                goto cleanup;
            }

            /* If the string is empty, set the default test command */
            if (json_object_get_string_len(tmp) == 0)
                ctx->header->compilation.test = strdup(PACKER_TEST_DEF);
            else
                ctx->header->compilation.test = strdup(json_object_get_string(tmp));
        }
        else if (strcmp(name, PACKER_CONF_COMP_INST_TOKEN) == 0)
        {
            /* Member type is unexpected */
            if (json_object_get_type(tmp) != json_type_string)
            {
                SET_ERR(ERR_BAD_JSON_TYPE);
                SET_ERR_STR_FMT("Bad JSON type for %s, expected a string", name);
                goto cleanup;
            }

            /* If the string is empty, set the default install command */
            if (json_object_get_string_len(tmp) == 0)
                ctx->header->compilation.install = strdup(PACKER_INST_DEF);
            else
                ctx->header->compilation.install = strdup(json_object_get_string(tmp));
        }
        else if (strcmp(name, PACKER_CONF_COMP_ENV_TOKEN) == 0)
        {
            /* Member type is unexpected */
            if (json_object_get_type(tmp) != json_type_array)
            {
                SET_ERR(ERR_BAD_JSON_TYPE);
                SET_ERR_STR_FMT("Bad JSON type for %s, expected an array", name);
                goto cleanup;
            }

            /* Iterate over each member of the array */
            for (size_t i = 0; i < json_object_array_length(tmp); i++)
            {
                struct json_object      *array_ent, *array_tmp;
                vector_string_t         *opt = NULL;

                /* Get the current member ID */
                array_ent = json_object_array_get_idx(tmp, i);
                if (json_object_get_type(array_ent) == json_type_object)
                {
                    struct json_object_iterator     it_arr, it_arr_end;

                    /* Get the first and last member */
                    it_arr = json_object_iter_begin(array_ent);
                    it_arr_end = json_object_iter_end(array_ent);

                    /* Iterate over each member */
                    while (!json_object_iter_equal(&it_arr, &it_arr_end))
                    {
                        /* Get the value */
                        array_tmp = json_object_iter_peek_value(&it_arr);

                        /* Member type is unexpected */
                        if (json_object_get_type(array_tmp) != json_type_string)
                        {
                            SET_ERR(ERR_BAD_JSON_TYPE);
                            SET_ERR_STR("Bad JSON type in the env array, expected string");
                            goto cleanup;
                        }

                        /* Get the value */
                        opt = vector_string_init(json_object_iter_peek_name(&it_arr),
                            json_object_get_string(array_tmp));

                        /* Add it to the context */
                        list_add(ctx->header->compilation.env, opt, sizeof(*opt));
                        free(opt);

                        /* Iterate over next member */
                        json_object_iter_next(&it_arr);
                    }
                }
                else if (json_object_get_type(array_ent) == json_type_string)
                {
                    /* Get the value */
                    opt = vector_string_init(NULL, json_object_get_string(array_ent));

                    /* Add it to the context */
                    list_add(ctx->header->compilation.env, opt, sizeof(*opt));
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

        /* Get next member */
        json_object_iter_next(&it);

    }
    return true;

cleanup:
    packer_header_comp_free(ctx->header);
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
        SET_ERR_STR("Bad JSON type for the dependencies, expected an array");
        return false;
    }

    /* Iterate over each member of the array */
    for (len = json_object_array_length(obj), i = 0; i < len; i++)
    {
        /* Get the current ID of the member */
        tmp = json_object_array_get_idx(obj, i);

        /* Member got an unexpected type */
        if (json_object_get_type(tmp) != json_type_string)
        {
            SET_ERR(ERR_BAD_JSON_TYPE);
            SET_ERR_STR("Bad JSON type for the dependencies array, expected a string");
            goto cleanup;
        }

        /* Add it to the context */
        list_add(ctx->header->dependencies.list, (char *)json_object_get_string(tmp),
            json_object_get_string_len(tmp) + 1);
    }

    return true;
cleanup:
    packer_header_deps_free(ctx->header);
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
        SET_ERR_STR("Bad JSON type for the package section, expected an object");
        return false;
    }

    /* Get the first and last member */
    it = json_object_iter_begin(obj);
    it_end = json_object_iter_end(obj);

    /* Iterating over each member */
    while (!json_object_iter_equal(&it, &it_end))
    {
        /* Get name and value */
        name = json_object_iter_peek_name(&it);
        tmp = json_object_iter_peek_value(&it);

        if (strcmp(name, PACKER_CONF_PACKAGE_NAME_TOKEN) == 0)
        {
            /* Member type is unexpected */
            if (json_object_get_type(tmp) != json_type_string)
            {
                SET_ERR(ERR_BAD_JSON_TYPE);
                SET_ERR_STR_FMT("Wrong type for %s, expected a string", name);
                goto cleanup;
            }

            /* Add it to the context */
            ctx->__pkg_name = strdup(json_object_get_string(tmp));
        }
        else if (strcmp(name, PACKER_CONF_PACKAGE_VERSION_TOKEN) == 0)
        {
            /* Member type is unexpected */
            if (json_object_get_type(tmp) != json_type_string)
            {
                SET_ERR(ERR_BAD_JSON_TYPE);
                SET_ERR_STR_FMT("Wrong type for %s, expected a string", name);
                goto cleanup;
            }

            /* Add it to the context */
            ctx->__pkg_version = strdup(json_object_get_string(tmp));
        }
        else if (strcmp(name, PACKER_CONF_PACKAGE_DESC_TOKEN) == 0)
        {
            /* Member type is unexpected */
            if (json_object_get_type(tmp) != json_type_string)
            {
                SET_ERR(ERR_BAD_JSON_TYPE);
                SET_ERR_STR_FMT("Wrong type for %s, expected a string", name);
                goto cleanup;
            }

            /* Add it to the context */
            ctx->__pkg_desc = strdup(json_object_get_string(tmp));
        }
        else if (strcmp(name, PACKER_CONF_PACKAGE_SBU_TOKEN) == 0)
        {
            /* Member type is unexpected */
            if (json_object_get_type(tmp) != json_type_double
                && json_object_get_type(tmp) != json_type_int)
            {
                SET_ERR(ERR_BAD_JSON_TYPE);
                SET_ERR_STR_FMT("Wrong type for %s, expected a int / double", name);
                goto cleanup;
            }

            /* Add it to the context */
            ctx->__pkg_sbu = json_object_get_double(tmp);
        }
        else if (strcmp(name, PACKER_CONF_PACKAGE_CATEG_TOKEN) == 0)
        {
            /* Member type is unexpected */
            if (json_object_get_type(tmp) != json_type_string)
            {
                SET_ERR(ERR_BAD_JSON_TYPE);
                SET_ERR_STR_FMT("Wrong type for %s, expected a string", name);
                goto cleanup;
            }

            /* Add it to the context */
            ctx->__pkg_categ = strdup(json_object_get_string(tmp));
        }
        else if (strcmp(name, PACKER_CONF_PACKAGE_INST_SIZE_TOKEN) == 0)
        {
            /* Member type is unexpected */
            if (json_object_get_type(tmp) != json_type_double
                && json_object_get_type(tmp) != json_type_int)
            {
                SET_ERR(ERR_BAD_JSON_TYPE);
                SET_ERR_STR_FMT("Wrong type for %s, expected a string", name);
                goto cleanup;
            }

            /* Add it to the context */
            ctx->__pkg_inst_size = json_object_get_double(tmp);
        }
        /* Wrong token */
        else
        {
            SET_ERR(ERR_BAD_JSON);
            SET_ERR_STR_FMT("Unknown JSON token: %s", name);
            goto cleanup;
        }

        /* Get next member */
        json_object_iter_next(&it);
    }
    return true;

cleanup:
    packer_header_package_free(ctx->header);
    return false;
}

MPX_STATIC bool packer_read_config_file(packer_t *ctx) {
    struct json_object_iterator     it, it_end;
    const char                      *name;

    assert(ctx != NULL);

    /* Get first and last member */
    it = json_object_iter_begin(ctx->json);
    it_end = json_object_iter_end(ctx->json);

    ctx->header = packer_header_init();

    /* Iterating over each member */
    while (!json_object_iter_equal(&it, &it_end))
    {
        /* Get the member's name */
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
            SET_ERR_STR_FMT("Unknown global JSON token: %s", name);
            goto cleanup;
        }

        /* Get next member */
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
        SET_ERR_STR_FMT("Can't go to the directory %s", ctx->str);
        goto error;
    }

    /* Get the JSON object */
    ctx->json = json_object_from_file(PACKER_DEF_CONF_FN);
    if (ctx->json == NULL)
    {
        SET_ERR(ERR_BAD_JSON);
        SET_ERR_STR_FMT("Error at parsing JSON in: %s", PACKER_DEF_CONF_FN);
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
    vector_string_t     *opt = NULL;
    const char          *tmp_str = NULL;
    u32_t               list_len = 0, conf_len = 0;
    double              sbu, inst_size;

    /* Write 3 bytes magic */
    fprintf(fd, PACKER_MPX_MAGIC);

    fprintf(fd, "%s%c", h->package.name, 0);
    fprintf(fd, "%s%c", h->package.version, 0);
    fprintf(fd, "%s%c", h->package.description, 0);
    fprintf(fd, "%s%c", h->package.categ, 0);

    /* Endianness of the SBU */
    sbu = htonl(h->package._sbu);
    fwrite(&sbu, sizeof(sbu), 1, fd);

    /* Endianness of the installation size */
    inst_size = htonl(h->package.inst_size);
    fwrite(&inst_size, sizeof(inst_size), 1, fd);

    /* Endianness of the configuration list len */
    conf_len = htonl(list_size(h->compilation.configure));
    fwrite(&conf_len, sizeof(u32_t), 1, fd);

    /* If configure is not null, write it to the archive */
    if (list_size(h->compilation.configure) != 0)
    {
        list_for_each(h->compilation.configure, tmp, opt) {
            if (opt->str1 != NULL)
                fprintf(fd, "%s:%s%c", opt->str1, opt->str2, 0);
            else
                fprintf(fd, "%s%c", opt->str2, 0);
        }
    }
    fprintf(fd, "%s%c", STR_OR_EMPTY(h->compilation.make), 0);
    fprintf(fd, "%s%c", STR_OR_EMPTY(h->compilation.test), 0);
    fprintf(fd, "%s%c", STR_OR_EMPTY(h->compilation.install), 0);

    /* Endianness of the environnement list len */
    conf_len = htonl(list_size(h->compilation.env));
    fwrite(&conf_len, sizeof(u32_t), 1, fd);

    if (list_size(h->compilation.env) != 0)
    {
        list_for_each(h->compilation.env, tmp, opt)
        {
            if (opt->str1 != NULL)
                fprintf(fd, "%s:%s%c", opt->str1, opt->str2, 0);
            else
                fprintf(fd, "%s%c", opt->str2, 0);
        }
    }

    /* Endianness of the dependencies list */
    list_len = htonl(list_size(h->dependencies.list));
    fwrite(&list_len, sizeof(u32_t), 1, fd);

    list_for_each(h->dependencies.list, tmp, tmp_str) {
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

    /* Add the dir to the linked list, to begin with */
    list_add(dirs, (void *)dir_name, strlen(dir_name) + 1);

    /* Iterating while there are dir to read in */
    list_for_each(dirs, tmp, dir) {
        if (read_files_from_dir(dir, &files_list, &dirs) == false)
            goto error;

        if (list_size(files_list) == 0)
            continue;

        /* For each file found, write it to the archive */
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

        /* Cleanup the written files */
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

    /* Create the archive file */
    fd = fopen(archive_path, "w+");
    if (fd == NULL)
    {
        SET_ERR(ERR_OPEN);
        SET_ERR_STR_FMT("Can't open file %s", archive_path);
        return false;
    }

    /* Write header */
    write_package_header(fd, ctx);

    /* Write srcs/ */
    if (write_packer_sources(fd, ctx, PACKER_SRC_DIR) == false)
        goto error;

    /* Write patches/ */
    if (write_packer_sources(fd, ctx, PACKER_PATCH_DIR) == false)
        goto error;

    /* Write scripts/ */
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
    double                      sbu, inst_size;

    if (ctx == NULL)
        return ret;

    ctx->__pkg_name = strdup(file + ret);
    if (ctx->__pkg_name == NULL)
        goto cleanup;

    ret += strlen(ctx->__pkg_name) + 1;

    ctx->__pkg_version = strdup(file + ret);
    if (ctx->__pkg_version == NULL)
        goto cleanup;

    ret += strlen(ctx->__pkg_version) + 1;

    ctx->__pkg_desc = strdup(file + ret);
    if (ctx->__pkg_desc == NULL)
        goto cleanup;

    ret += strlen(ctx->__pkg_desc) + 1;

    ctx->__pkg_categ = strdup(file + ret);
    if (ctx->__pkg_categ == NULL)
        goto cleanup;

    ret += strlen(ctx->__pkg_categ) + 1;

    memcpy(&sbu, file + ret, sizeof(sbu));
    ctx->__pkg_sbu = ntohl(sbu);
    ret += sizeof(sbu);

    memcpy(&inst_size, file + ret, sizeof(inst_size));
    ctx->__pkg_inst_size = ntohl(inst_size);
    ret += sizeof(inst_size);

    return ret;

cleanup:
    SET_ERR(ERR_MEMORY);
    if (ctx != NULL)
        packer_header_package_free(ctx->header);
    return 0;
}

MPX_STATIC bool read_conf_opt(char *file, mlist_t **list, int *ret) {
    u32_t               size;
    vector_string_t     *opt = NULL;
    char                *tmp = NULL, *ptr = NULL;

    memcpy(&size, file + *ret, sizeof(size));
    size = ntohl(size);
    *ret += sizeof(size);

    /* Iterate over each configure parameter */
    for (u32_t i = 0; i < size; i++)
    {
        tmp = file + *ret;
        *ret += strlen(tmp) + 1;

        /* Find the ':' (option:value delimeter) */
        ptr = strchr(tmp, ':');
        if (ptr == NULL)
        {
            /* If there is none, simply copy the string */
            opt = vector_string_init(NULL, tmp);
            if (opt == NULL)
            {
                SET_ERR(ERR_MEMORY);
                goto cleanup;
            }
        }
        else
        {
            /* Copy the name */
            *ptr = 0;
            opt = vector_string_init(tmp, ptr + 1);
            if (opt == NULL)
            {
                SET_ERR(ERR_MEMORY);
                goto cleanup;
            }
        }

        /* Add the option to the context */
        list_add(*(list), opt, sizeof(*opt));
        free(opt);
    }

    return true;

cleanup:
    list_free(*(list), vector_string_free);
    *list = NULL;
    return false;
}

MPX_STATIC int read_package_header_compilation(char *file, packer_t *ctx)
{
    int                         ret = 0;

    if (ctx == NULL)
        return ret;

    if (read_conf_opt(file, &ctx->header->compilation.configure, &ret) != true)
        goto cleanup;

    ctx->header->compilation.make = strdup(file + ret);
    if (ctx->header->compilation.make == NULL)
    {
        SET_ERR(ERR_MEMORY);
        goto cleanup;
    }

    ret += strlen(ctx->header->compilation.make) + 1;

    ctx->header->compilation.test = strdup(file + ret);
    if (ctx->header->compilation.test == NULL)
    {
        SET_ERR(ERR_MEMORY);
        goto cleanup;
    }
    ret += strlen(ctx->header->compilation.test) + 1;

    ctx->header->compilation.install = strdup(file + ret);
    if (ctx->header->compilation.install == NULL)
    {
        SET_ERR(ERR_MEMORY);
        goto cleanup;
    }
    ret += strlen(ctx->header->compilation.install) + 1;

    if (read_conf_opt(file, &ctx->header->compilation.env, &ret) != true)
        goto cleanup;

    return ret;

cleanup:
    if (ctx != NULL)
        packer_header_comp_free(ctx->header);
    return 0;
}

MPX_STATIC int read_package_header_dependencies(const char *file, packer_t *ctx) {
    int                         ret = 0;
    u32_t                       list_len = 0;
    char                        *tmp = NULL;

    /* Get the list size */
    memcpy(&list_len, file, sizeof(u32_t));
    /* Endianness */
    list_len = ntohl(list_len);
    ret += sizeof(u32_t);

    /* Iterate over each string */
    for (u32_t i = 0; i < list_len; i++)
    {
        /* Copy the dependency string */
        tmp = strdup(file + ret);
        if (tmp == NULL)
        {
            SET_ERR(ERR_MEMORY);
            goto cleanup;
        }
        ret += strlen(tmp) + 1;

        /* Add it to the context */
        list_add(ctx->header->dependencies.list, tmp, strlen(tmp) + 1);
        free(tmp);
    }

    return ret;

cleanup:
    if (ctx != NULL)
        packer_header_deps_free(ctx->header);
    return 0;
}

MPX_STATIC bool read_package_header(char *file_content, packer_t *ctx, int *s_ret)
{
    int  ret = 0, tmp;

    if (file_content == NULL || ctx == NULL)
    {
        SET_ERR(ERR_BAD_PTR);
        return false;
    }

    /* Test the 3 bytes magic */
    if (strncmp(file_content, PACKER_MPX_MAGIC, sizeof(PACKER_MPX_MAGIC) - 1) != 0)
    {
        SET_ERR(ERR_NOT_A_PACKAGE);
        SET_ERR_STR("Not a MPX archive");
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
    if (ctx != NULL)
    {
        packer_header_free(ctx->header);
        ctx->header = NULL;
    }
    return false;
}

bool packer_read_archive_header(packer_t *ctx) {
    char    *archive = NULL;
    bool    ret;
    int     cur = 0;

    if (ctx == NULL)
        return false;

    if (ctx->type != PACKER_TYPE_ARCHIVE)
        return false;

    /* Get file content */
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

    if (ctx == NULL)
        return false;

    /* Save the current working directory */
    getcwd(old_pwd, sizeof(old_pwd));

    if (ctx->type != PACKER_TYPE_ARCHIVE)
    {
        SET_ERR(ERR_BAD_ARCHIVE_TYPE);
        return false;
    }

    /* Open the archive */
    fd = open(ctx->str, O_RDONLY);
    if (fd == -1)
    {
        SET_ERR(ERR_OPEN);
        SET_ERR_STR_FMT("Can't open archive '%s'", ctx->str);
        return false;
    }

    /* Get the archive binary content */
    archive = mpm_read_file_from_fd(fd);
    size = mpm_get_file_size_from_fd(fd);

    if (!read_package_header(archive, ctx, &t_ctr))
        goto cleanup;

    ctr = t_ctr;

    /* Open the extract directory */
    DIR *p_dir = opendir(dir);

    /* If it's non-existent, create it */
    if (p_dir == NULL)
        mkdir(dir, S_IRWXU | S_IRWXG | S_IRWXO);
    else
        closedir(p_dir);

    /* Go to the extract directory */
    if (chdir(dir) == -1)
    {
        SET_ERR(ERR_CHDIR_FAILED);
        SET_ERR_STR_FMT("Cannot go in the directory '%s'\n", dir);
        goto cleanup;
    }

    /* Create the package directory name */
    asprintf(&ctx->out_dir, "%s/%s-%s/", dir, ctx->__pkg_name, ctx->__pkg_version);

    /* Create the actual directory */
    if (mkdir(ctx->out_dir, S_IRWXU | S_IRWXG | S_IRWXO) == -1)
    {
        SET_ERR(ERR_MKDIR_FAILED);
        SET_ERR_STR_FMT("Cannot create directory '%s'\n", ctx->out_dir);
        goto cleanup;
    }

    /* Go in the directory */
    chdir(ctx->out_dir);

    /* Read each file, and dump them to the disk */
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
