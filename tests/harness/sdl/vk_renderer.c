#include "vk_renderer.h"

typedef struct vk_renderer_s
{
  const mdo_allocator_t *alloc;
} vk_renderer_t;

void
vk_renderer_finalize (void *data)
{
  vk_renderer_t *vk_renderer = (vk_renderer_t *)data;

  const mdo_allocator_t *alloc = vk_renderer->alloc;

  mdo_allocator_free (alloc, vk_renderer);
}

mdo_ui_renderer_t *
create_vk_renderer (const mdo_allocator_t *alloc)
{
  vk_renderer_t *vk_renderer
      = mdo_allocator_malloc (alloc, sizeof (vk_renderer_t));
  vk_renderer->alloc = alloc;

  mdo_ui_renderer_params_t params = { .finalize = vk_renderer_finalize };

  mdo_ui_renderer_t *ui_renderer;
  mdo_result_t result
      = mdo_ui_renderer_create (&ui_renderer, alloc, &params, vk_renderer);

  return ui_renderer;
}
