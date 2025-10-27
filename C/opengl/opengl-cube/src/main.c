#ifdef __APPLE__
#define GL_SILENCE_DEPRECATION
#endif

#define FILETOOLS_IMPLEMENTATION
#define SHADERTOOLS_IMPLEMENTATION
#include "shadertools.h"

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#define GLAD_GL_IMPLEMENTATION
#include <glad/gl.h>

#include <stdio.h>
#include <stdlib.h>

int main(void) {
  if (!glfwInit()) {
    fprintf(stderr, "GLFW failed to initialize\n");
    return EXIT_FAILURE;
  }

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

  GLFWwindow *window = glfwCreateWindow(500, 500, "Hello Cube", NULL, NULL);
  if (window == NULL) {
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

//   int transformLoc = glGetUniformLocation(shader_program, "transform");

  while (!glfwWindowShouldClose(window)) {
    glClearColor(135.0f / 255.0f, 206.0f / 255.0f, 235.0f / 255.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(shader_program);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glfwTerminate();
  return EXIT_SUCCESS;
}
