#include "filetools.h"
#include "gltools.h"
#include <glad/gl.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define using(cleanup_func) __attribute__((cleanup(cleanup_func)))

void strcleanup(char **str) {
  if (*str != NULL) {
    free(*str);
	*str = NULL;
  }
}


#define SHADER_OK 0       /* Success */
#define SHADER_INVALID -1 /* Invalid parameters */
#define SHADER_ERROR -2   /* General error */
#define SHADER_GLERROR -2 /* General error */

static int shadersrc(const char *path, char **srcptr) {
  if (path == NULL || srcptr == NULL) {
    return SHADER_INVALID;
  }

  FILE *file = fopen(path, "rb");
  if (!file) {
    perror("Failed to open shader file");
    return SHADER_ERROR;
  }

  char *data = NULL;
  size_t size = 0;
  if (File_readall(file, &data, &size) < 0) {
    fprintf(stderr, "Error: Failed to read shader file.\n");
    return SHADER_ERROR;
  }

  *srcptr = data;
  data = NULL;

  fclose(file);
  file = NULL;

  return SHADER_OK;
}

static int shadercompile(GLenum type, const char *shader_src,
                         GLuint *shader_idptr) {
  if (shader_src == NULL || shader_idptr == NULL) {
    return SHADER_INVALID;
  }

  GLuint shader_id = glCreateShader(type);
  if (shader_id == 0) {
    fprintf(stderr, "Failed to create shader.\n");
    return SHADER_GLERROR;
  }
  const GLchar *source_code = shader_src;
  glShaderSource(shader_id, 1, &source_code, NULL);
  glCheckError();

  glCompileShader(shader_id);
  glCheckError();

  GLint compile_status = 0;
  glGetShaderiv(shader_id, GL_COMPILE_STATUS, &compile_status);
  glCheckError();

  if (compile_status == GL_FALSE) {
    GLint info_log_length = 0;
    glGetShaderiv(shader_id, GL_INFO_LOG_LENGTH, &info_log_length);
    glCheckError();

    GLchar info_log[info_log_length];
    glGetShaderInfoLog(shader_id, info_log_length, NULL, info_log);
    glCheckError();

    fprintf(stderr, "Shader compilation failed:\n%s", info_log);
    return SHADER_ERROR;
  }

  *shader_idptr = shader_id;

  return SHADER_OK;
}

static int shaderlink(GLuint vertex, GLuint fragment, GLuint *program_idptr) {
  GLuint program_id = glCreateProgram();
  if (program_id == 0) {
    fprintf(stderr, "Failed to create shader program.\n");
    return SHADER_GLERROR;
  }

  glAttachShader(program_id, vertex);
  glCheckError();

  glAttachShader(program_id, fragment);
  glCheckError();

  glLinkProgram(program_id);
  glCheckError();

  GLint link_status = GL_FALSE;
  glGetProgramiv(program_id, GL_LINK_STATUS, &link_status);
  glCheckError();
  if (link_status == GL_FALSE) {
    GLint info_log_length = 0;
    glGetProgramiv(program_id, GL_INFO_LOG_LENGTH, &info_log_length);
    glCheckError();

    GLchar info_log[info_log_length];
    glGetProgramInfoLog(program_id, info_log_length, NULL, info_log);
    glCheckError();

    fprintf(stderr, "Shader link error: %s\n", info_log);

    return SHADER_GLERROR;
  }

  *program_idptr = program_id;

  return SHADER_OK;
}

static void shadercleanup(GLuint *shader_idptr) {
  if (*shader_idptr == 0) {
    return;
  }

  glDeleteShader(*shader_idptr);
  *shader_idptr = 0;
  glCheckError();
}

int shader_load_program(const char *vertex_path, const char *fragment_path,
                        GLuint *program_idptr) {
  using(strcleanup) char *vertex_src = NULL;
  if (shadersrc(vertex_path, &vertex_src) < 0) {
    return -1;
  }

  using(shadercleanup) GLuint vertex_id = 0;
  if (shadercompile(GL_VERTEX_SHADER, vertex_src, &vertex_id) < 0) {
    return -1;
  }

  using(strcleanup) char *fragment_src = NULL;
  if (shadersrc(fragment_path, &fragment_src) < 0) {
    return -1;
  }

  using(shadercleanup) GLuint fragment_id = 0;
  if (shadercompile(GL_FRAGMENT_SHADER, fragment_src, &fragment_id) < 0) {
    return -1;
  }

  GLuint program_id = 0;
  if (shaderlink(vertex_id, fragment_id, &program_id) < 0) {
    return -1;
  }

  *program_idptr = program_id;

  return 0;
}
