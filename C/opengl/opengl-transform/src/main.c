#ifdef __APPLE__
#define GL_SILENCE_DEPRECATION
#endif

#define FILETOOLS_IMPLEMENTATION
#define SHADERTOOLS_IMPLEMENTATION
#include "shadertools.h"
#define GLFW_INCLUDE_NONE
#include "GLFW/glfw3.h"
#define GLAD_GL_IMPLEMENTATION
#include <glad/gl.h>

#include <cglm/cglm.h>

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

static void glCheckError_(const char *file, int line) {
  GLenum gl_error;
  while ((gl_error = glGetError()) != GL_NO_ERROR) {
    const char *gl_error_name = NULL;
    switch (gl_error) {
    case GL_INVALID_ENUM:
      gl_error_name = "GL_INVALID_ENUM";
      break;
    case GL_INVALID_VALUE:
      gl_error_name = "GL_INVALID_VALUE";
      break;
    case GL_INVALID_OPERATION:
      gl_error_name = "GL_INVALID_OPERATION";
      break;
    case GL_INVALID_FRAMEBUFFER_OPERATION:
      gl_error_name = "GL_INVALID_FRAMEBUFFER_OPERATION";
      break;
    case GL_OUT_OF_MEMORY:
      gl_error_name = "GL_OUT_OF_MEMORY";
      break;
    default:
      gl_error_name = "GL_UNKNOWN";
    }

    fprintf(stderr, "%s(%d) | %s (%d)\n", gl_error_name, gl_error, file, line);
  }
}

#define glCheckError() glCheckError_(__FILE__, __LINE__)

int main(void) {
  int major, minor, revision;
  glfwGetVersion(&major, &minor, &revision);
  printf("Initializing GLFW v%i.%i.%i\n", major, minor, major);

  if (!glfwInit()) {
    fprintf(stderr, "GLFW failed to initialize\n");
    return EXIT_FAILURE;
  }

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

  GLFWwindow *window =
      glfwCreateWindow(500, 500, "Hello Transform", NULL, NULL);
  if (!window) {
    glfwTerminate();
    return EXIT_FAILURE;
  }

  glfwMakeContextCurrent(window);

  int version = gladLoadGL(glfwGetProcAddress);
  if (!version) {
    fprintf(stderr, "GLAD Error: failed to initialize\n");
    glfwTerminate();
    return EXIT_FAILURE;
  }

  printf("Loaded OpenGL v%d.%d\b", GLAD_VERSION_MAJOR(version),
         GLAD_VERSION_MINOR(version));

  GLuint shader_program = 0;
  if (loadShaderProgram("res/shaders/basic.vert", "res/shaders/basic.frag",
                        &shader_program) < 0) {
    glfwTerminate();
    return EXIT_FAILURE;
  }

  GLuint VAO;
  glGenVertexArrays(1, &VAO);
  glBindVertexArray(VAO);

  float vertices[] = {
      -0.5f, 0.5f,  0.0f, // Top Left
      0.5f,  0.5f,  0.0f, // Top Right
      0.5f,  -0.5f, 0.0f, // Bottom Right
      -0.5f, -0.5f, 0.0f  // Bottom Left
  };

  GLuint VBO;
  glGenBuffers(1, &VBO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);
  glCheckError();

  int transformLoc = glGetUniformLocation(shader_program, "transform");

  float x = 0.0f, y = 0.0f;

  while (!glfwWindowShouldClose(window)) {
    double time = glfwGetTime();

    glClearColor(135.0f / 255.0f, 206.0f / 255.0f, 235.0f / 255.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // Calculate transform
    mat4 transform;
    glm_mat4_identity(transform);
    glm_scale(transform, (vec3){0.5f, 0.5f, 0.0f});

    x = sin(time);
    y = cos(time);

    glm_rotated_at(transform, (vec3){x, y, 0.0f}, time,
                   (vec3){0.0f, 1.0f, 1.0f});

    // glm_translate(transform, (vec3){x, y, 0.0f});

    glUseProgram(shader_program);
    glUniformMatrix4fv(transformLoc, 1, GL_FALSE, (const float *)transform);

    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glfwTerminate();
  return EXIT_SUCCESS;
}
