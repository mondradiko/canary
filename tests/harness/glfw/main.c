#include <stdio.h>

#define GLFW_INCLUDE_ES2
#include "GLFW/glfw3.h"

#include <mdo-utils/result.h>

#include "draw_list.h"
#include "gles_renderer.h"
#include "panel.h"
#include "script.h"

static int
log_error (const char *message)
{
  fprintf (stderr, "%s\n", message);
  return -1;
}

int
run_harness (const char *filename)
{
  int error_code = 0;

  const mdo_allocator_t *alloc = mdo_default_allocator ();
  mdo_result_t result = MDO_SUCCESS;

  GLFWwindow *window = NULL;
  canary_script_t *script = NULL;
  canary_panel_t *panel = NULL;
  gles_renderer_t *ren = NULL;
  canary_draw_list_t *draw_list = NULL;

  if (!glfwInit ())
    return log_error ("failed to init glfw");

  glfwWindowHint (GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
  glfwWindowHint (GLFW_CONTEXT_VERSION_MAJOR, 2);
  glfwWindowHint (GLFW_CONTEXT_VERSION_MINOR, 0);

  window = glfwCreateWindow (800, 600, "ES Test", NULL, NULL);
  if (!window)
    {
      error_code = log_error ("failed to create window");
      goto error;
    }

  glfwMakeContextCurrent (window);

  printf ("GL_VERSION  : %s\n", glGetString (GL_VERSION));
  printf ("GL_RENDERER : %s\n", glGetString (GL_RENDERER));

  result = canary_script_create (&script, alloc);
  if (!mdo_result_success (result))
    {
      LOG_ERR ("failed to create UI script");
      error_code = 1;
      goto error;
    }

  result = canary_script_load (script, filename);
  if (!mdo_result_success (result))
    {
      LOG_ERR ("failed to load UI script");
      error_code = 1;
      goto error;
    }

  result = canary_panel_create (&panel, alloc);
  if (!mdo_result_success (result))
    {
      LOG_ERR ("failed to create UI panel");
      error_code = 1;
      goto error;
    }

  result = canary_draw_list_create (&draw_list, alloc);
  if (!mdo_result_success (result))
    {
      LOG_ERR ("failed to create UI draw list");
      error_code = 1;
      goto error;
    }

  canary_panel_set_draw_list (panel, draw_list);

  canary_panel_key_t panel_key;
  if (canary_script_bind_panel (script, panel, &panel_key))
    {
      LOG_ERR ("failed to bind UI panel");
      error_code = 1;
      goto error;
    }

  if (gles_renderer_create (&ren, panel))
    {
      LOG_ERR ("failed to create renderer");
      error_code = 1;
      goto error;
    }

  while (!glfwWindowShouldClose (window))
    {
      glfwPollEvents ();
      glClear (GL_COLOR_BUFFER_BIT);
      glfwSwapBuffers (window);
    }

error:
  if (ren)
    gles_renderer_delete (ren);

  if (draw_list)
    canary_draw_list_delete (draw_list);

  if (panel)
    {
      canary_script_unbind_panel (script, panel_key);
      canary_panel_delete (panel);
    }

  if (script)
    canary_script_delete (script);

  glfwTerminate ();
  return 0;
}

int
main (int argc, const char *argv[])
{
  if (argc != 2)
    {
      fprintf (stderr, "Usage:\n  %s [script.wasm]\n", argv[0]);
      return 1;
    }

  const char *filename = argv[1];
  int result = run_harness (filename);
  mdo_result_cleanup ();

  return result;
}
