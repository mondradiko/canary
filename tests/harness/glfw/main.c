#include <stdio.h>

#define GLFW_INCLUDE_ES2
#include "GLFW/glfw3.h"

#include <mdo-utils/result.h>

static int
log_error (const char *message)
{
  fprintf (stderr, "%s\n", message);
  return -1;
}

int
run_harness (const char *filename)
{
  if (!glfwInit ())
    return log_error ("failed to init glfw");

  glfwWindowHint (GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
  glfwWindowHint (GLFW_CONTEXT_VERSION_MAJOR, 2);
  glfwWindowHint (GLFW_CONTEXT_VERSION_MINOR, 0);

  GLFWwindow *window = glfwCreateWindow (800, 600, "ES Test", NULL, NULL);
  if (!window)
    return log_error ("failed to create window");

  glfwMakeContextCurrent (window);

  printf ("GL_VERSION  : %s\n", glGetString (GL_VERSION));
  printf ("GL_RENDERER : %s\n", glGetString (GL_RENDERER));

  while (!glfwWindowShouldClose (window))
    {
      glfwPollEvents ();
      glClear (GL_COLOR_BUFFER_BIT);
      glfwSwapBuffers (window);
    }

  glfwTerminate ();
  return 0;
}

int
main (int argc, const char *argv[])
{
  if (argc != 2)
    {
      fprintf (stderr, "Usage:\n  %s [ui_script.wasm]\n", argv[0]);
      return 1;
    }

  const char *filename = argv[1];
  int result = run_harness (filename);
  mdo_result_cleanup ();

  return result;
}
