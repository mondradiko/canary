#include <stdio.h>

#define GLFW_INCLUDE_ES2
#include "GLFW/glfw3.h"

#include <mdo-utils/result.h>

#include "draw_list.h"
#include "gles_renderer.h"
#include "panel.h"
#include "script.h"

typedef struct window_userdata_s
{
  canary_script_t *script;
  canary_panel_t *panel;
  canary_panel_key_t panel_key;
} window_userdata_t;

static int
log_error (const char *message)
{
  fprintf (stderr, "%s\n", message);
  return -1;
}

static void
send_input_event (GLFWwindow *window, canary_input_event_t event)
{
  window_userdata_t *userdata = glfwGetWindowUserPointer (window);

  double cx;
  double cy;
  glfwGetCursorPos (window, &cx, &cy);

  int width;
  int height;
  glfwGetWindowSize (window, &width, &height);

  float coords[2];
  coords[0] = (cx / width) * 2.0 - 1.0;
  coords[1] = 1.0 - (cy / height) * 2.0;

  canary_script_on_input (userdata->script, userdata->panel_key, event, coords);
}

static void
cursor_position_callback (GLFWwindow *window, double xpos, double ypos)
{
  canary_input_event_t event;

  if (glfwGetMouseButton (window, GLFW_MOUSE_BUTTON_LEFT))
    event = CANARY_DRAG;
  else
    event = CANARY_HOVER;

  send_input_event (window, event);
}

static void
mouse_button_callback (GLFWwindow *window, int button, int action, int mods)
{
  if (button != GLFW_MOUSE_BUTTON_LEFT)
    return;

  canary_input_event_t event;

  if (action)
    event = CANARY_SELECT;
  else
    event = CANARY_DESELECT;

  send_input_event (window, event);
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

  window_userdata_t userdata;

  if (!glfwInit ())
    return log_error ("failed to init glfw");

  glfwWindowHint (GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
  glfwWindowHint (GLFW_CONTEXT_VERSION_MAJOR, 2);
  glfwWindowHint (GLFW_CONTEXT_VERSION_MINOR, 0);

  window = glfwCreateWindow (800, 600, "Canary Test Harness", NULL, NULL);
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

  userdata.script = script;
  userdata.panel = panel;
  userdata.panel_key = panel_key;

  glfwSetWindowUserPointer (window, &userdata);
  glfwSetCursorPosCallback (window, cursor_position_callback);
  glfwSetMouseButtonCallback (window, mouse_button_callback);

  double last_tick = glfwGetTime ();

  while (!glfwWindowShouldClose (window))
    {
      glfwPollEvents ();

      double this_tick = glfwGetTime ();
      float dt = this_tick - last_tick;
      last_tick = this_tick;

      canary_draw_list_clear (draw_list);
      canary_script_update (script, dt);

      glClear (GL_COLOR_BUFFER_BIT);
      gles_renderer_render_frame (ren);
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
