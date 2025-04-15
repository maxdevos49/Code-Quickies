#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>

int main(int argc, const char *argv[])
{
    if (argc != 2)
    {
        fprintf(stderr, "Usage:\n\tfsize <filepath>\n");
        return EXIT_FAILURE;
    }

    const char *path = argv[1];
    if (strnlen(path, PATH_MAX) >= PATH_MAX)
    {
        fprintf(stderr, "Invalid path: Path exceeds maximum path length.");
        return EXIT_FAILURE;
    }

    FILE *file = NULL;
    file = fopen(path, "rb");
    if (file == NULL)
    {
        perror("Could not open file");
        return EXIT_FAILURE;
    }

    if (fseeko(file, 0, SEEK_END) != 0)
    {
        perror("Could not seek file end.");
        goto cleanup;
    }

    off_t position = ftello(file);
    if (position < 0)
    {
        perror("Could not read file position");
        goto cleanup;
    }

    printf("%lld bytes\n", position);
    fclose(file);
    return EXIT_SUCCESS;

cleanup:
    fclose(file);
    return EXIT_FAILURE;
}
