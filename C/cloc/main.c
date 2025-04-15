#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include <stdbool.h>

#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))
char buffer[4096];

int main(int argc, char *argv[])
{

    if (argc != 2)
    {
        fprintf(stderr, "Usage:\n\tcloc <filepath>\n");
        return EXIT_FAILURE;
    }

    const char *path = argv[1];
    size_t path_len = strnlen(path, PATH_MAX);
    if (path_len == 0 || path_len == PATH_MAX)
    {
        fprintf(stderr, "Invalid path\n");
        return EXIT_FAILURE;
    }

    FILE *file = fopen(path, "r");
    if (file == NULL)
    {
        perror("Failed to open file");
        return EXIT_FAILURE;
    }

    size_t lines = 0;
    while (true)
    {
        size_t bytes_read = fread(buffer, sizeof(char), ARRAY_SIZE(buffer), file);
        if (bytes_read != ARRAY_SIZE(buffer) && ferror(file) != 0)
        {
            perror("Failed to read file");
            goto cleanup;
        }

        if (bytes_read == 0)
        {
            if (lines > 0)
                lines++;

            break;
        }

        for (size_t i = 0; i < bytes_read; i++)
        {
            if (buffer[i] == '\n')
                lines++;
        }

        if (feof(file) != 0)
        {
            lines++;
            break;
        }
    }

    fclose(file);
    printf("%lu\n", lines);

    return EXIT_SUCCESS;

cleanup:
    fclose(file);
    return EXIT_FAILURE;
}
