/**
 * File ToolBox v0.1 September 20, 2025
 * Author: Max DeVos github:maxdevos49
 *
 * Header only library for file operations which the standard lib does not
 * provide for or safer alternatives.
 *
 * In only one source file do:
 * #define FILETOOLS_IMPLEMENTATION
 * #include "filetools.h"
 *
 */
#ifndef FILETOOLS_H
#define FILETOOLS_H 1

#include <stddef.h>
#include <stdio.h>

int readall(FILE *in, char **dataptr, size_t *sizeptr);

#endif // FILETOOLS_H

#ifdef FILETOOLS_IMPLEMENTATION

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#ifndef READALL_CHUNK
#define READALL_CHUNK 262144
#endif

#define READALL_OK 0       /* Success */
#define READALL_INVALID -1 /* Invalid parameters */
#define READALL_ERROR -2   /* Stream error */
#define READALL_TOOMUCH -3 /* To much input */
#define READALL_NOMEM -4   /* Out of memory */

int readall(FILE *in, char **dataptr, size_t *sizeptr) {
  if (in == NULL || dataptr == NULL || sizeptr == NULL) {
    return READALL_INVALID;
  }

  if (ferror(in)) {
    return READALL_ERROR;
  }

  char *data = NULL, *temp = NULL;
  size_t size = 0;
  size_t capacity = 0;

  while (1) {
    if (size + READALL_CHUNK + 1 > capacity) {
      capacity = size + READALL_CHUNK + 1;

      // Integer overflow check.
      if (capacity <= size) {
        free(data);
        data = NULL;
        return READALL_TOOMUCH;
      }

      temp = realloc(data, capacity);
      if (temp == NULL) {
        free(data);
        data = NULL;
        return READALL_NOMEM;
      }

      data = temp;
      temp = NULL;
    }

    size_t bytes = fread(data + size, 1, READALL_CHUNK, in);
    if (bytes == 0) {
      break;
    }

    size += bytes;
  }

  if (ferror(in)) {
    free(data);
    data = NULL;
    return READALL_ERROR;
  }

  temp = realloc(data, size + 1);
  if (temp == NULL) {
    free(data);
    data = NULL;
    return READALL_NOMEM;
  }

  data = temp;
  temp = NULL;
  data[size] = '\0';

  *dataptr = data;
  *sizeptr = size;

  return READALL_OK;
}

#endif // FILETOOLS_IMPLEMENTATION
