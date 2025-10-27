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
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

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

  stbi_set_flip_vertically_on_load(1);
  int width, height, nrChannels;
  unsigned char *data =
      stbi_load("res/textures/blocks.png", &width, &height, &nrChannels, 0);
  if (data == NULL) {
    fprintf(stderr, "Failed to load texture.\n");
    glfwTerminate();
    return EXIT_FAILURE;
  }

  GLuint texture;
  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA,
               GL_UNSIGNED_BYTE, data);
  glGenerateMipmap(GL_TEXTURE_2D);

  stbi_image_free(data);
  data = NULL;

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
      0.5f,  0.5f,  0.0f, 1.0f/3.0f, 1.0f, // top right
      0.5f,  -0.5f, 0.0f, 1.0f/3.0f, 0.0f, // bottom right
      -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, // bottom left
      -0.5f, 0.5f,  0.0f, 0.0f, 1.0f  // top left
  };

  GLuint VBO;
  glGenBuffers(1, &VBO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);

  unsigned int indices[] = {0, 1, 2, 0, 2, 3};

  GLuint EBO;
  glGenBuffers(1, &EBO);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices,
               GL_STATIC_DRAW);

  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
                        (void *)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);

  glCheckError();
  //   int transformLoc = glGetUniformLocation(shader_program, "transform");

  glUseProgram(shader_program);
  glBindTexture(GL_TEXTURE_2D, texture);
  glBindVertexArray(VAO);

  while (!glfwWindowShouldClose(window)) {
    glClearColor(135.0f / 255.0f, 206.0f / 255.0f, 235.0f / 255.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glCheckError();

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glfwTerminate();
  return EXIT_SUCCESS;
}
