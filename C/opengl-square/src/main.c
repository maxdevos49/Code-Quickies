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
#include <stdio.h>
#include <stdlib.h>

int main(void) {
  int major, minor, revision;
  glfwGetVersion(&major, &minor, &revision);
  printf("Initializing GLFW v%i.%i.%i\n", major, minor, revision);

  if (!glfwInit()) {
    fprintf(stderr, "GLFW failed to initialize\n");
    return EXIT_FAILURE;
  }

  // MacOS only supports the opengl versions 3.2 -> 4.1 and its core profile
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT,
                 GL_TRUE); // Removes deprecated features of older versions

  GLFWwindow *window = glfwCreateWindow(500, 500, "Hello Square", NULL, NULL);
  if (!window) {
    glfwTerminate();
    return EXIT_FAILURE;
  }

  glfwMakeContextCurrent(window);

  int version = gladLoadGL(glfwGetProcAddress);
  if (!version) {
    fprintf(stderr, "GLAD Error: Failed to  initialize\n");
    glfwTerminate();
    return EXIT_FAILURE;
  }

  printf("Loaded OpenGL v%d.%d\n", GLAD_VERSION_MAJOR(version),
         GLAD_VERSION_MINOR(version));

  GLuint shader_program = 0;
  if (loadShaderProgram("res/shaders/basic.vert", "res/shaders/basic.frag",
                        &shader_program) < 0) {
    glfwTerminate();
    return EXIT_FAILURE;
  }

  //
  // Square
  //

  GLuint VAO, VBO, EBO;

  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
  glGenBuffers(1, &EBO);

  float vertices[] = {
      0.5f,  0.5f,  0.0f, // Top right
      0.5f,  -0.5f, 0.0f, // Bottom right
      -0.5f, -0.5f, 0.0f, // Bottom left
      -0.5f, 0.5f,  0.0f  // Top left
  };

  glBindVertexArray(VAO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  unsigned int indices[] = {
	0, 1, 3, // First Triangle
	3, 2, 1 // Second Triangle
};

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices,
               GL_STATIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);

  //   glBindVertexArray(0);

  while (!glfwWindowShouldClose(window)) {
    glClearColor(135.0f / 255.0f, 206.0f / 255.0f, 235.0f / 255.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(shader_program);
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    glfwSwapBuffers(window);

    glfwPollEvents();
  }

  glfwTerminate();
  return EXIT_SUCCESS;
}
