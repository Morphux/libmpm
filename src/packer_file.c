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

#include <packer_file.h>

int packer_file_free(void *magic) {
    packer_file_t   *file = magic;

    if (file != NULL)
    {
        free(file->fn);
        free(file->content);
    }
    return 1;
}

packer_file_t *packer_file_init(const char *file, const char *dir) {
    packer_file_t   *ret = NULL;

    ret = calloc(1, sizeof(*ret));
    if (ret == NULL)
        return NULL;

    ret->fn = malloc(strlen(file) + strlen(dir) + 1);

    if (ret->fn == NULL)
    {
        free(ret);
        return NULL;
    }

    if (strcpy(ret->fn, dir) == NULL)
        goto error;

    if (strcat(ret->fn, file) == NULL)
        goto error;

    ret->sum[0] = 0;

    return ret;

error:
    packer_file_free(ret);
    free(ret);
    return NULL;
}

bool read_files_from_dir(const char *dir_name, mlist_t **files, mlist_t **dirs) {
    DIR             *dir = opendir(dir_name);
    struct dirent   *dinfo = NULL;
    packer_file_t   *file = NULL;

    /* Directory not here is okay */
    if (dir == NULL)
        return true;

    while ((dinfo = readdir(dir)))
    {
        if (strlen(dinfo->d_name) > 0 && dinfo->d_name[0] == '.')
            continue ;

        if (dinfo->d_type == DT_DIR)
        {
            size_t tmp = strlen(dinfo->d_name);

            dinfo->d_name[tmp] = '/';
            dinfo->d_name[tmp + 1] = '\0';
        }

        file = packer_file_init(dinfo->d_name, dir_name);
        if (file == NULL)
            goto error;

        if (dinfo->d_type == DT_DIR)
        {
            list_add((*dirs), file->fn, strlen(file->fn) + 1);
            packer_file_free(file);
        }
        else
        {
            if (get_file_information(file) == false)
            {
                packer_file_free(file);
                free(file);
                goto error;
            }
            list_add((*files), file, sizeof(*file));
        }
        free(file);
    }

    closedir(dir);
    return true;

error:
    closedir(dir);
    return false;
}

bool get_file_information(packer_file_t *file) {
    z_stream        stream;
    char            *file_content = NULL;
    char            *chunk = NULL;
    off_t           file_size;

    file_size = mpm_get_file_size_from_fn(file->fn);
    file_content = mpm_read_file_from_fn(file->fn);
    if (file_content == NULL)
    {
        file->file_size = 0;
        file->content = NULL;
        return true;
    }

    crypto_hash_sha256((unsigned char *)file->sum,
        (const unsigned char *)file_content,
        file_size);

    chunk = malloc(file_size * 2);
    if (chunk == NULL)
    {
        free(file_content);
        return false;
    }

    /* Compress file */
    stream.zalloc = NULL;
    stream.zfree = NULL;
    stream.opaque = NULL;
    stream.avail_in = file_size;
    stream.next_in = (Bytef *)file_content;
    stream.avail_out = file_size * 2;
    stream.next_out = (Bytef *)chunk;

    deflateInit(&stream, Z_BEST_COMPRESSION);
    deflate(&stream, Z_FINISH);
    deflateEnd(&stream);

    file->file_size = stream.total_out;
    file->content = malloc(file->file_size);
    if (file->content == NULL)
    {
        free(file_content);
        free(chunk);
        return false;
    }

    memcpy(file->content, chunk, file->file_size);

    free(file_content);
    free(chunk);
    return true;
}

packer_file_t *read_packer_file_from_binary(const char *content, off_t *ctr) {
    packer_file_t   *file = NULL;
    z_stream        stream;

    file = malloc(sizeof(*file));
    if (file == NULL)
        return NULL;

    file->content = NULL;

    /* File name */
    file->fn = strdup(content + *ctr);
    if (file->fn == NULL)
    {
        free(file);
        return NULL;
    }

    *ctr += strlen(file->fn) + 1;

    /* File content size */
    memcpy(&file->file_size, content + *ctr, sizeof(file->file_size));
    *ctr += sizeof(file->file_size);

    if (file->file_size > 0) {
        /* File hash */
        memcpy(file->sum, content + *ctr, crypto_hash_sha256_BYTES);
        *ctr += crypto_hash_sha256_BYTES;

        /* Actual file content */
        stream.zalloc = NULL;
        stream.zfree = NULL;
        stream.opaque = NULL;
        stream.avail_in = file->file_size;
        stream.avail_out = file->file_size * 2;

        inflateInit(&stream);

        file->content = malloc(file->file_size * 2);
        if (file->content == NULL)
        {
            inflateEnd(&stream);
            free(file->fn);
            free(file);
            return NULL;
        }

        /* Decompress file */
        stream.next_in = (unsigned char *)content + *ctr;
        stream.next_out = (unsigned char *)file->content;

        inflate(&stream, Z_NO_FLUSH);
        file->content[stream.total_out] = '\0';
        inflateEnd(&stream);


        *ctr += file->file_size;
    }
    return file;
}

bool packer_file_to_disk(packer_file_t *file) {
    char        *tmp = NULL;
    FILE        *fd = NULL;

    for (tmp = file->fn + 1; *tmp; tmp++)
    {
        if (*tmp == '/')
        {
            *tmp = 0;
            if (mkdir(file->fn, S_IRWXU) == -1 && errno != EEXIST)
            {
                *tmp = '/';
                return false;
            }
            *tmp = '/';
        }
    }

    fd = fopen(file->fn, "w+");
    if (fd == NULL)
        return false;

    fprintf(fd, "%s", file->content);
    fclose(fd);
    return true;
}
