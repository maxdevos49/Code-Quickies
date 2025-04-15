#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <limits.h>

int main(void)
{
    printf("Enter maximum number: ");

    int64_t maximum;
    if (scanf("%llu", &maximum) != 1)
    {
        fprintf(stderr, "Invalid input. Please enter a positive number.\n");
        return EXIT_FAILURE;
    }

    if (maximum < 0)
    {
        fprintf(stderr, "Invalid input. Please enter a positive number.\n");
        return EXIT_FAILURE;
    }

    if (maximum == 0)
    {
        printf("0\n");
        return EXIT_SUCCESS;
    }

    uint64_t previous = 0, next = 1;
    while (previous < (int64_t)maximum)
    {
        printf("%llu\n", previous);

        if (next > ULLONG_MAX - previous)
        {
            fprintf(stderr, "Integer overflow detected.\n");
            return EXIT_FAILURE;
        }

        uint64_t result = previous + next;
        previous = next;
        next = result;
    }

    return EXIT_SUCCESS;
}
