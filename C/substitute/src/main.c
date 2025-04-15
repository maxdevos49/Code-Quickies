#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <stdbool.h>
#include <string.h>

#define ERROR_PREFIX "substitute: \033[0;31merror\033[0m "
#define FATAL(msg)                                 \
    do                                             \
    {                                              \
        fprintf(stderr, ERROR_PREFIX "%s\n", msg); \
        goto cleanup;                              \
    } while (0);

#define FATAL_E(msg)                               \
    do                                             \
    {                                              \
        fprintf(stderr, ERROR_PREFIX "%s\n", msg); \
        perror(NULL);                              \
        goto cleanup;                              \
    } while (0);

struct Options
{
    char *template_path;
    char *pattern;
    size_t pattern_len;
    char *value;
};

void usage(void)
{
    fprintf(stderr, "OVERVIEW: File pattern substitution\n\n");
    fprintf(stderr, "Usage: substitute <filepath> [options]\n\n");
    fprintf(stderr, "OPTIONS:\n");
    fprintf(stderr, "\t-p <pattern>=<value>         Defines a pattern and value to be used for substitution.\n");
}

bool validate_path(char *path)
{
    if (path == NULL || *path == '\0' || strlen(path) >= PATH_MAX)
        return false;

    return true;
}

#define BUFFER_SIZE 4096
char buffer[BUFFER_SIZE + 1];
int main(int argc, char *argv[])
{
    FILE *template = NULL;
    struct Options options = {.template_path = argv[1]};

    if (argc < 2)
    {
        usage();
        return EXIT_FAILURE;
    }

    for (int i = 2; i < argc; i++)
    {
        char *flag = argv[i];

        if (strcmp("-p", flag) == 0)
        {
            if (options.pattern != NULL)
                FATAL("Only one instance of the -p flag is supported.\n");

            if (i + 1 < argc)
            {
                char *flag_value = strdup(argv[++i]);
                if (flag_value == NULL)
                {
                    FATAL_E("Failed to copy string");
                }

                options.pattern = strtok(flag_value, "=");
                options.value = strtok(NULL, "=");
                if (options.value == NULL)
                    FATAL("malformed value for argument: -p. Usage: -p <pattern>=<value>\n");

                options.pattern_len = strlen(options.pattern);
            }
            else
            {
                FATAL("missing value for argument: -p. Usage: -p <pattern>=<value>\n");
            }
        }
        else
        {
            fprintf(stderr, ERROR_PREFIX "unknown argument: '%s'\n", flag);
            goto cleanup;
        }
    }

    if (!validate_path(options.template_path))
        FATAL("Invalid input path.\n");

    if (!options.pattern)
        FATAL("Missing pattern\n")

    if (!options.value)
        FATAL("Missing value\n")

    template = fopen(options.template_path, "r");
    if (template == NULL)
        FATAL_E("Failed to open template file.");

    while (true)
    {
        size_t bytes_read = fread(buffer, sizeof(char), BUFFER_SIZE, template);
        if (bytes_read != BUFFER_SIZE && ferror(template) != 0)
            FATAL_E("Failed to read template file\n");

        if (bytes_read == 0)
            break;

        // Terminate buffer so we dont run into infinity.
        buffer[bytes_read] = '\0';

        size_t offset = 0;
        size_t position = 0;
        while (position < bytes_read)
        {
            // Probably could make a optimized strncmp which bails the moment
            // something is different but this works.
            if (buffer[position] == options.pattern[0] && strncmp(options.pattern, buffer + position, options.pattern_len) == 0)
            {
                // NULL terminate up to where we read so fprintf does not run
                // off into infinity
                buffer[position] = '\0';

                // Print everything before the pattern
                fprintf(stdout, "%s", buffer + offset);

                // Print pattern value
                fprintf(stdout, "%s", options.value);

                offset = position + options.pattern_len;
                position += options.pattern_len;
            }
            else
            {
                position++;
            }
        }

        // Print anything remaining. (Might be nothing)
        fprintf(stdout, "%s", buffer + offset);

        if (feof(template) != 0)
            break;
    }

    free(options.pattern);
    fclose(template);

    return EXIT_SUCCESS;
cleanup:
    if (options.pattern)
        free(options.pattern);
    if (template)
        fclose(template);

    return EXIT_FAILURE;
}
