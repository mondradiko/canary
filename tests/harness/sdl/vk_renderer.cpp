extern "C"
{
#include "vk_renderer.h"
}

#include <SDL2/SDL_vulkan.h>

#include "VkBootstrap.h"

struct vk_renderer_s
{
  vkb::Instance instance;
  VkSurfaceKHR surface;
  vkb::Device device;
  vkb::Swapchain swapchain;

  VkQueue graphics_queue;
  VkQueue present_queue;
};

vk_renderer_t *
vk_renderer_create (const mdo_allocator_t *alloc, SDL_Window *window)
{
  vk_renderer_t *vk_renderer = new vk_renderer_t;

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

  if (SDL_Vulkan_CreateSurface (window, vk_renderer->instance.instance,
                                &vk_renderer->surface)
      != SDL_TRUE)
    {
      LOG_ERR ("failed to create SDL-Vulkan window surface: %s",
               SDL_GetError ());
      return NULL;
    }

  vkb::PhysicalDeviceSelector phys_device_selector (vk_renderer->instance);
  auto phys_device_ret
      = phys_device_selector.set_surface (vk_renderer->surface).select ();
  if (!phys_device_ret)
    {
      LOG_ERR ("failed to find Vulkan physical device: %s",
               phys_device_ret.error ().message ().c_str ());
      return NULL;
    }

  vkb::DeviceBuilder device_builder{ phys_device_ret.value () };
  auto device_ret = device_builder.build ();
  if (!device_ret)
    {
      LOG_ERR ("failed to create Vulkan logical device: %s",
               device_ret.error ().message ().c_str ());
      return NULL;
    }

  vk_renderer->device = device_ret.value ();

  /* TODO(marceline-cramer): swapchain recreation *on resize* */
  /*vkb::SwapchainBuilder swapchain_builder{ vk_renderer->device };
  auto swap_ret = swapchain_builder.build ();
  if (!swap_ret)
    {
      LOG_ERR ("failed to create swapchain: %s",
               swap_ret.error ().message ().c_str ());
      return NULL;
    }*/

  /* for deleting old swapchain later */
  /* vkb::destroy_swapchain (vk_renderer->swapchain); */

  /* vk_renderer->swapchain = swap_ret.value (); */

  /*auto gq = vk_renderer->device.get_queue (vkb::QueueType::graphics);
  if (!gq.has_value ())
    {
      LOG_ERR ("failed to get graphics queue: %s",
               gq.error ().message ().c_str ());
      return NULL;
    }

  vk_renderer->graphics_queue = gq.value ();

  auto pq = vk_renderer->device.get_queue (vkb::QueueType::present);
  if (!pq.has_value ())
    {
      LOG_ERR ("failed to get present queue: %s",
               pq.error ().message ().c_str ());
      return NULL;
    }

  vk_renderer->present_queue = pq.value ();*/

  return vk_renderer;
}

void
vk_renderer_delete (vk_renderer_t *vk_renderer)
{
  vkb::destroy_swapchain (vk_renderer->swapchain);
  vkb::destroy_device (vk_renderer->device);
  vkb::destroy_surface (vk_renderer->instance, vk_renderer->surface);
  vkb::destroy_instance (vk_renderer->instance);

  delete vk_renderer;
}
