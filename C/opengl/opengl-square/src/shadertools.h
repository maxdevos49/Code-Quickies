/**
 * Shader ToolBox v0.1 September 20, 2025
 * Author: Max DeVos github:maxdevos49
 *
 * Header only library for opengl shader operations.
 *
 * In only one source file do:
 * #define SHADERTOOLS_IMPLEMENTATION
 * #include "shadertools.h"
 *
 */
#ifndef SHADERTOOLS_H
#define SHADERTOOLS_H 1

#include <glad/gl.h>

int loadShaderProgram(const char *vertex_path, const char *fragment_path,
                      GLuint *program_idptr);

#endif // SHADERTOOLS_H

#define SHADERTOOLS_IMPLEMENTATION
#ifdef SHADERTOOLS_IMPLEMENTATION

#include "filetools.h"
#include <glad/gl.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

static int loadShaderSource(const char *path, char **srcptr) {
  if (path == NULL || srcptr == NULL) {
    return -1;
  }

  FILE *file = fopen(path, "rb");
  if (!file) {
    perror("Failed to open shader file");
    return -1;
  }

  char *data = NULL;
  size_t size = 0;
  if (readall(file, &data, &size) < 0) {
    fprintf(stderr, "Error: Failed to read shader file.\n");
    return -1;
  }

  *srcptr = data;
  data = NULL;

  fclose(file);
  file = NULL;

  return 0;
}

static int compileShader(GLenum type, const char *shader_src,
                         GLuint *shader_idptr) {
  if (shader_src == NULL || shader_idptr == NULL) {
    return -1;
  }

  GLuint shader_id = glCreateShader(type);
  if (shader_id == 0) {
    fprintf(stderr, "Failed to create shader.\n");
    return -1;
  }
  const GLchar *source_code = shader_src;
  glShaderSource(shader_id, 1, &source_code, NULL);
  //   glCheckError();

  glCompileShader(shader_id);
  //   glCheckError();

  GLint compile_status = 0;
  glGetShaderiv(shader_id, GL_COMPILE_STATUS, &compile_status);
  //   glCheckError();

  if (compile_status == GL_FALSE) {
    GLint info_log_length = 0;
    glGetShaderiv(shader_id, GL_INFO_LOG_LENGTH, &info_log_length);
    // glCheckError();

    GLchar info_log[info_log_length];
    glGetShaderInfoLog(shader_id, info_log_length, NULL, info_log);
    // glCheckError();

    glDeleteShader(shader_id);
    fprintf(stderr, "Shader compilation failed:\n%s", info_log);
    return -1;
  }

  *shader_idptr = shader_id;

  return 0;
}

static int linkShaderProgram(GLuint vertex, GLuint fragment,
                             GLuint *program_idptr) {
  GLuint program_id = glCreateProgram();
  if (program_id == 0) {
    fprintf(stderr, "Failed to create shader program.\n");
    return -1;
  }

  glAttachShader(program_id, vertex);
  //   glCheckError();

  glAttachShader(program_id, fragment);
  //   glCheckError();

  glLinkProgram(program_id);
  //   glCheckError();

  GLint link_status = GL_FALSE;
  glGetProgramiv(program_id, GL_LINK_STATUS, &link_status);
  //   glCheckError();
  if (link_status == GL_FALSE) {
    GLint info_log_length = 0;
    glGetProgramiv(program_id, GL_INFO_LOG_LENGTH, &info_log_length);
    // glCheckError();

    GLchar info_log[info_log_length];
    glGetProgramInfoLog(program_id, info_log_length, NULL, info_log);
    // glCheckError();

    fprintf(stderr, "Shader link error: %s\n", info_log);
    glDeleteProgram(program_id);
    return -1;
  }

  *program_idptr = program_id;

  return 0;
}

int loadShaderProgram(const char *vertex_path, const char *fragment_path,
                      GLuint *program_idptr) {
  char *vertex_src = NULL;
  if (loadShaderSource(vertex_path, &vertex_src) < 0) {
    fprintf(stderr, "Failed to load vertex shaders source: %s\n", vertex_path);
    return -1;
  }

  GLuint vertex_id = 0;
  if (compileShader(GL_VERTEX_SHADER, vertex_src, &vertex_id) < 0) {
    fprintf(stderr, "Failed to compile vertex shaders source: %s\n",
            vertex_path);
    free(vertex_src);
    vertex_src = NULL;
    return -1;
  }

  free(vertex_src);
  vertex_src = NULL;

  char *fragment_src = NULL;
  if (loadShaderSource(fragment_path, &fragment_src) < 0) {
    fprintf(stderr, "Failed to load fragment shaders source: %s\n",
            vertex_path);

    return -1;
  }

  GLuint fragment_id = 0;
  if (compileShader(GL_FRAGMENT_SHADER, fragment_src, &fragment_id) < 0) {
    fprintf(stderr, "Failed to compile fragment shaders source: %s\n",
            vertex_path);
    free(fragment_src);
    fragment_src = NULL;
    glDeleteShader(vertex_id);
    vertex_id = 0;
    return -1;
  }

  free(fragment_src);
  fragment_src = NULL;

  GLuint program_id = 0;
  if (linkShaderProgram(vertex_id, fragment_id, &program_id) < 0) {
    fprintf(stderr, "Failed to link shader program\n");
    glDeleteShader(vertex_id);
    vertex_id = 0;
    glDeleteShader(fragment_id);
    fragment_id = 0;
    return -1;
  }

  glDeleteShader(vertex_id);
  vertex_id = 0;
  glDeleteShader(fragment_id);
  fragment_id = 0;

  *program_idptr = program_id;

  return 0;
}

#endif // SHADERTOOLS_IMPLEMENTATION
