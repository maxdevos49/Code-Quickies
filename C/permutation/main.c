#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

// https://www.cprogramming.com/challenges/permute.html

// Continue with because below is wrong:
// - https://stackoverflow.com/questions/2799078/permutation-algorithm-without-recursion-java
// - https://en.wikipedia.org/wiki/Factorial_number_system

int main(void)
{
    printf("Enter a word: ");

    char word[256] = {0};
    if (scanf("%s", word) != 1) // This is probably not safe
    {
        fprintf(stderr, "Invalid input\n");
        return EXIT_FAILURE;
    }

    size_t word_length = strlen(word);
    size_t total_permutations = 0;
    for (size_t i = 1; i < word_length; i++)
    {
        for (size_t j = 0; j < word_length; j++)
        {
            for (size_t h = 0; h < word_length; h++)
            {
                putc(word[(j + h) % word_length], stdout);
            }

            total_permutations++;
            putc('\n', stdout);
        }

        // Incrementally swap next char with front of string to work through
        // all combinations.
        char temp = word[0];
        word[0] = word[i];
        word[i] = temp;
    }

    printf("Total Permutations: %lu\n", total_permutations);

    return EXIT_SUCCESS;
}
