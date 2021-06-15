extern "C"
{
#include "vk_renderer.h"
}

#include "VkBootstrap.h"

struct vk_renderer_s
{
  const mdo_allocator_t *alloc;

  vkb::Instance instance;
};

vk_renderer_t *
vk_renderer_create (const mdo_allocator_t *alloc)
{
  vk_renderer_t *vk_renderer = static_cast<vk_renderer_t *> (
      mdo_allocator_malloc (alloc, sizeof (vk_renderer_t)));
  vk_renderer->alloc = alloc;

  vkb::InstanceBuilder instance_builder;
  auto instance_ret = instance_builder.use_default_debug_messenger ()
                          .request_validation_layers ()
                          .build ();

  if (!instance_ret)
    {
      LOG_ERR ("failed to initialize Vulkan instance: %s",
               instance_ret.error ().message ().c_str ());
      return NULL;
    }

  vk_renderer->instance = instance_ret.value ();

  return vk_renderer;
}

void
vk_renderer_delete (vk_renderer_t *vk_renderer)
{
  const mdo_allocator_t *alloc = vk_renderer->alloc;

  vkb::destroy_instance (vk_renderer->instance);

  mdo_allocator_free (alloc, vk_renderer);
}
