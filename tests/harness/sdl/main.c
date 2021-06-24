#include <SDL2/SDL.h>
#include <SDL_timer.h>

#include "draw_list.h"
#include "panel.h"
#include "script.h"
#include "vk_renderer.h"

SDL_Window *
create_sdl_window ()
{
  SDL_Window *window = SDL_CreateWindow (
      "Mondradiko UI", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 800,
      600, SDL_WINDOW_SHOWN | SDL_WINDOW_VULKAN | SDL_WINDOW_RESIZABLE);

  if (!window)
    {
      LOG_ERR ("failed to create SDL window: %s", SDL_GetError ());
    }

  return window;
}

int
poll_sdl_events (SDL_Window *window, canary_panel_t *panel)
{
  int error = 0;

  SDL_Event e;
  while (SDL_PollEvent (&e) != 0)
    {
      switch (e.type)
        {
        case SDL_QUIT:
          {
            error = 1;
            break;
          }

        default:
          break;
        }
    }

  return error;
}

int
run_harness (const char *filename)
{
  int error_code = 0;

  const mdo_allocator_t *alloc = mdo_default_allocator ();
  mdo_result_t result;

  canary_script_t *ui_script = NULL;
  canary_panel_t *panel = NULL;
  vk_renderer_t *renderer = NULL;
  canary_draw_list_t *ui_draw = NULL;

  SDL_Window *window = NULL;

  result = canary_script_create (&ui_script, alloc);
  if (!mdo_result_success (result))
    {
      LOG_ERR ("failed to create UI script");
      error_code = 1;
      goto error;
    }

  result = canary_script_load (ui_script, filename);
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

  result = canary_draw_list_create (&ui_draw, alloc);
  if (!mdo_result_success (result))
    {
      LOG_ERR ("failed to create UI draw list");
      error_code = 1;
      goto error;
    }

  canary_panel_set_draw_list (panel, ui_draw);

  canary_panel_key_t panel_key;
  result = canary_script_bind_panel (ui_script, panel, &panel_key);
  if (!mdo_result_success (result))
    {
      LOG_ERR ("failed to bind UI panel");
      error_code = 1;
      goto error;
    }

  window = create_sdl_window ();
  if (!window)
    {
      LOG_ERR ("failed to create panel window");
      error_code = 1;
      goto error;
    }

  renderer = vk_renderer_create (alloc, panel, window);
  if (!renderer)
    {
      LOG_ERR ("failed to create UI renderer");
      error_code = 1;
      goto error;
    }

  canary_panel_set_draw_list (panel, ui_draw);

  double clock_frequency = SDL_GetPerformanceFrequency ();
  uint64_t last_tick = SDL_GetPerformanceCounter ();

  while (!poll_sdl_events (window, panel))
    {
      uint64_t this_tick = SDL_GetPerformanceCounter ();
      float dt = ((double)(this_tick - last_tick)) / clock_frequency;
      last_tick = this_tick;

      canary_draw_list_clear (ui_draw);
      canary_script_update (ui_script, dt);
      vk_renderer_render_frame (renderer);
    }

error:
  if (renderer)
    vk_renderer_delete (renderer);

  if (window)
    SDL_DestroyWindow (window);

  if (ui_draw)
    canary_draw_list_delete (ui_draw);

  if (panel)
    {
      canary_script_unbind_panel (ui_script, panel_key);
      canary_panel_delete (panel);
    }

  if (ui_script)
    canary_script_delete (ui_script);

  return error_code;
}

int
main (int argc, const char *argv[])
{
  if (argc != 2)
    {
      fprintf (stderr, "Usage:\n%s [ui_script.wasm]\n", argv[0]);
      return 1;
    }

  const char *filename = argv[1];
  int result = run_harness (filename);
  mdo_result_cleanup ();

  return 0;
}
