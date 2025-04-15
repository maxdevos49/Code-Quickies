#include <dirent.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

int main(void) {
  DIR *dir = opendir(".");
  if (dir == NULL) {
    perror("Failed to open directory");
    return EXIT_FAILURE;
  }

  while (true) {
    struct dirent *entry = readdir(dir);
    if (entry == NULL) {
      break;
    }

    printf("%s\n", entry->d_name);
  }

  (void)closedir(dir);
  return EXIT_SUCCESS;
}
