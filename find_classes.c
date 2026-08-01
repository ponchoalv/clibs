#include <dirent.h>
#include <stdio.h>
#include <string.h>

#define ARENA_IMPLEMENTATION
#include "arena.h"

#define PCH_REALLOC(A, P, N) (A) == NULL ? realloc(P, N) : arena_realloc(A, P, N, sizeof(void *))
#define PCH_FREE(A, P) (A) == NULL ? free(P) : ((void)0)

#define PCH_DS_IMPLEMENTATION
#include "pch_ds.h"

static void parse_dirs(const char *path, char ***includes, const char *pattern, size_t len, char **ignores,
                size_t ignores_len);

static int check_ignores(const char *name, char **ignores, size_t len);

typedef struct
{
    char *key;
    int value;
} StringMap;

int main(const int argc, char **argv)
{
    Arena arena = arena_create(mega_byte * 256);
    char **includes = NULL;
    StringMap *unique = NULL;
    size_t i = 0;
    size_t len = 0;
    char **ignores = NULL;
    size_t ignores_len = 0;
    int *match = NULL;

    if (argc < 2)
    {
        printf("USAGE: ./find_classes <start_with_pattern> <ignores>\n");
        return 1;
    }
    else if (argc > 2)
    {
        ignores = argv + 2;
        ignores_len = argc - 2;
    }

    len = strlen(argv[1]);

    arrinit(includes, &arena);
    shinit(unique, &arena);

    parse_dirs(".", &includes, argv[1], len, ignores, ignores_len);
    printf("total matched: %zu\nLines:\n", arrlen(includes));
    for (i = 0; i < arrlen(includes); i++)
    {
        if (((match = shgetvp(unique, includes[i]))) && mfok(unique))
        {
            // printf("matched %d\n", *match);
            *match = *match + 1;
        }
        else
        {
            shput(unique, includes[i], 1);
        }
        printf("%s", includes[i]);
    }

    printf("\n------------\n\nunique entries: %zu\nall uniques: \n", arrlen(unique));
    for (i = 0; i < arrlen(unique); i++)
    {
        printf("%d -> %s", unique[i].value, unique[i].key);
    }

    printf("\n------------\n\n");
    printf("total matched: %zu\n", arrlen(includes));
    printf("unique entries: %zu\n", arrlen(unique));

    /* free resources */
    for (i = 0; i < arrlen(includes); i++)
    {
        free(includes[i]);
    }

    // arrfree(unique);
    // arrfree(includes);
    arena_destroy(&arena);
    return 0;
}

static int check_ignores(const char *name, char **ignores, const size_t len)
{
    size_t i = 0;
    if (ignores == NULL)
        return 0;
    for (i = 0; i < len; i++)
    {
        if (0 == strcmp(name, ignores[i]))
        {
            return 1;
        }
    }
    return 0;
}

void parse_dirs(const char *path, char ***includes, const char *pattern, size_t len, char **ignores, const size_t ignores_len)
{
    Arena ringArena = arena_create(mega_byte * 256);
    struct dirent *dp = NULL;
    char buff[4096];
    char *line = NULL;
    size_t linecap = 0;
    FILE *fd = NULL;
    char **paths = NULL;

    arrinit(paths, &ringArena);
    arrpush(paths, strdup(path));

    while (!arrempty(paths))
    {
        char *current_path = arrpop(paths);
        DIR *dir = opendir(current_path);
        if (dir)
        {
            while ((dp = readdir(dir)) != NULL)
            {
                if (strcmp(dp->d_name, ".") == 0 || strcmp(dp->d_name, "..") == 0 ||
                    check_ignores(dp->d_name, ignores, ignores_len))
                    continue;
                switch (dp->d_type)
                {
                case DT_DIR:
                    sprintf(buff, "%s/%s", current_path, dp->d_name);
                    arrpush(paths, strdup(buff));
                    break;
                case DT_REG:
                    sprintf(buff, "%s/%s", current_path, dp->d_name);
                    fd = fopen(buff, "r");
                    if (fd)
                    {
                        while (getline(&line, &linecap, fd) > 0)
                        {
                            if (strncmp(line, pattern, len) == 0)
                            {
                                arrpush(*includes, strdup(line));
                            }
                        }
                    }
                    else
                    {
                        printf("couldn't open file %s\n", buff);
                    }
                    if (fd)
                        fclose(fd);
                    break;
                case DT_UNKNOWN:
                case DT_FIFO:
                case DT_CHR:
                case DT_BLK:
                case DT_LNK:
                case DT_SOCK:
                case DT_WHT:
                default:
                    break;
                }
            }
            if (dir)
                closedir(dir);
        }
        else
        {
            printf("couldn't open directory %s\n", current_path);
        }
        if (current_path)
            free(current_path);
    }

    if (line)
        free(line);
    if (fd)
        fclose(fd);
    // arrfree(paths);
    arena_destroy(&ringArena);
}
