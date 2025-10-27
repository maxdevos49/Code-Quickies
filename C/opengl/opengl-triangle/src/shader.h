#ifndef __SHADER_H
#define __SHADER_H 1

#include <glad/gl.h>

int shader_load_program(const char *vertex_path, const char *fragment_path,
                        GLuint *program_idptr);

#endif
