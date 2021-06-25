#include <stdio.h>

#define GLFW_INCLUDE_ES2
#include "GLFW/glfw3.h"

int log_error(const char *message) {
  fprintf(stderr, "%s\n", message);
  return -1;
}

int main() {
  if (!glfwInit())
    return log_error("failed to init glfw");

  glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

  GLFWwindow *window = glfwCreateWindow(800, 600, "ES Test", NULL, NULL);
  if (!window)
    return log_error("failed to create window");

  glfwMakeContextCurrent(window);

  printf("GL_VERSION  : %s\n", glGetString(GL_VERSION));
  printf("GL_RENDERER : %s\n", glGetString(GL_RENDERER));

  while (!glfwWindowShouldClose(window)) {
    glfwPollEvents();
    glClear(GL_COLOR_BUFFER_BIT);
    glfwSwapBuffers(window);
  }

  glfwTerminate();
  return 0;
}
