#include "ui_renderer.h"

struct mdo_ui_renderer_s
{
  const mdo_allocator_t *alloc;
  void *userdata;
  mdo_ui_renderer_params_t params;
};

mdo_result_t
mdo_ui_renderer_create (mdo_ui_renderer_t **ui_renderer,
                        const mdo_allocator_t *alloc,
                        mdo_ui_renderer_params_t *params, void *userdata)
{
  mdo_ui_renderer_t *new_ui_renderer
      = mdo_allocator_malloc (alloc, sizeof (mdo_ui_renderer_t));
  *ui_renderer = new_ui_renderer;

  new_ui_renderer->alloc = alloc;
  new_ui_renderer->userdata = userdata;
  new_ui_renderer->params = *params;

  return MDO_SUCCESS;
}

void
mdo_ui_renderer_delete (mdo_ui_renderer_t *ui_renderer)
{
  const mdo_allocator_t *alloc = ui_renderer->alloc;

  ui_renderer->params.finalize (ui_renderer->userdata);

  mdo_allocator_free (alloc, ui_renderer);
}
