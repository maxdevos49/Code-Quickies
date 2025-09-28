#ifdef __APPLE__
#define GL_SILENCE_DEPRECATION
#endif

#define FILETOOLS_IMPLEMENTATION
// #include "filetools.h"
#define SHADERTOOLS_IMPLEMENTATION
#include "shadertools.h"
#define GLFW_INCLUDE_NONE
#include "GLFW/glfw3.h"
#define GLAD_GL_IMPLEMENTATION
#include <glad/gl.h>

#include <math.h>
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

  GLFWwindow *window = glfwCreateWindow(500, 500, "Hello Circle", NULL, NULL);
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
  // Circle
  //

  GLuint VAO;
  glGenVertexArrays(1, &VAO);
  glBindVertexArray(VAO);

  GLuint VBO;
  glGenBuffers(1, &VBO);

  float origin_x = 0.0f;
  float origin_y = 0.0f;
  float radius = 0.5;
  int segments = 40;

  float vertices[(segments + 2) * 3];

  // Origin
  vertices[0] = origin_x;
  vertices[1] = origin_y;
  vertices[2] = 0.0f;

  for (int i = 0; i < segments; i++) {
    float rad = 2 * M_PI * ((float)i / (float)segments);
    float x = radius * cos(rad);
    float y = radius * sin(rad);

    int idx = 3 + (i * 3);
    vertices[idx + 0] = x;
    vertices[idx + 1] = y;
    vertices[idx + 2] = 0.0f;
  }

  // Closing Point (Same as vertex[1])
  vertices[(segments + 1) * 3 + 0] = vertices[3];
  vertices[(segments + 1) * 3 + 1] = vertices[4];
  vertices[(segments + 1) * 3 + 2] = vertices[4];

  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);

  while (!glfwWindowShouldClose(window)) {
    glClearColor(135.0f / 255.0f, 206.0f / 255.0f, 235.0f / 255.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(shader_program);
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLE_FAN, 0, segments + 2);

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glfwTerminate();
  return EXIT_SUCCESS;
}
