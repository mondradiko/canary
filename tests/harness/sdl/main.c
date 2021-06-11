#include <SDL2/SDL.h>

#include "ui_panel.h"
#include "ui_script.h"
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
poll_sdl_events (SDL_Window *window, mdo_ui_panel_t *panel)
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
  const mdo_allocator_t *alloc = mdo_default_allocator ();
  mdo_result_t result;

  mdo_ui_script_t *ui_script;
  result = mdo_ui_script_create (&ui_script, alloc);
  if (!mdo_result_success (result))
    {
      LOG_ERR ("failed to create UI script");
      return 1;
    }

  result = mdo_ui_script_load (ui_script, filename);
  if (!mdo_result_success (result))
    {
      LOG_ERR ("failed to load UI script");
      mdo_ui_script_delete (ui_script);
      return 1;
    }

  mdo_ui_renderer_t *ui_renderer = create_vk_renderer (alloc);
  if (!ui_renderer)
    {
      LOG_ERR ("failed to create UI renderer");
      mdo_ui_script_delete (ui_script);
      return 1;
    }

  mdo_ui_panel_t *panel;
  result = mdo_ui_panel_create (&panel, alloc, ui_script);
  if (!mdo_result_success (result))
    {
      LOG_ERR ("failed to create UI panel");
      mdo_ui_script_delete (ui_script);
      return 1;
    }

  SDL_Window *window = create_sdl_window ();
  if (!window)
    {
      LOG_ERR ("failed to create panel window");
      mdo_ui_renderer_delete (ui_renderer);
      mdo_ui_script_delete (ui_script);
      return 1;
    }

  while (!poll_sdl_events (window, panel))
    ;

  if (window)
    SDL_DestroyWindow (window);

  if (panel)
    mdo_ui_panel_delete (panel);

  if (ui_renderer)
    mdo_ui_renderer_delete (ui_renderer);

  if (ui_script)
    mdo_ui_script_delete (ui_script);

  return 0;
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
