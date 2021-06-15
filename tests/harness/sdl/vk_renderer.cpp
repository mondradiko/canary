#include "vk_renderer.h"

struct vk_renderer_s
{
  const mdo_allocator_t *alloc;
};

vk_renderer_t *
vk_renderer_create (const mdo_allocator_t *alloc)
{
  vk_renderer_t *vk_renderer
      = mdo_allocator_malloc (alloc, sizeof (vk_renderer_t));
  vk_renderer->alloc = alloc;

  return vk_renderer;
}

void
vk_renderer_delete (vk_renderer_t *vk_renderer)
{
  const mdo_allocator_t *alloc = vk_renderer->alloc;

  mdo_allocator_free (alloc, vk_renderer);
}
