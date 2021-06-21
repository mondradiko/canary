extern "C"
{
#include "vk_renderer.h"
}

#include <SDL2/SDL_vulkan.h>

#include "VkBootstrap.h"

typedef struct frame_data_s
{
  VkCommandPool command_pool = VK_NULL_HANDLE;
  VkFence is_in_flight = VK_NULL_HANDLE;
  VkSemaphore on_available = VK_NULL_HANDLE;
  VkSemaphore on_finished = VK_NULL_HANDLE;
} frame_data_t;

struct vk_renderer_s
{
  mdo_ui_panel_t *panel;

  vkb::Instance instance;
  VkSurfaceKHR surface;
  vkb::Device device;
  vkb::Swapchain swapchain;

  VkQueue graphics_queue;
  VkQueue present_queue;

  VkRenderPass render_pass = VK_NULL_HANDLE;

  std::vector<VkImageView> image_views;
  std::vector<VkFramebuffer> framebuffers;

  std::vector<frame_data_t> frames;
  int current_frame = 0;
};

vk_renderer_t *
vk_renderer_create (const mdo_allocator_t *alloc, mdo_ui_panel_t *panel,
                    SDL_Window *window)
{
  vk_renderer_t *ren = new vk_renderer_t;

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

  ren->instance = instance_ret.value ();

  if (SDL_Vulkan_CreateSurface (window, ren->instance.instance, &ren->surface)
      != SDL_TRUE)
    {
      LOG_ERR ("failed to create SDL-Vulkan window surface: %s",
               SDL_GetError ());
      return NULL;
    }

  vkb::PhysicalDeviceSelector phys_device_selector (ren->instance);
  auto phys_device_ret
      = phys_device_selector.set_surface (ren->surface).select ();
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

  ren->device = device_ret.value ();

  /* TODO(marceline-cramer): swapchain recreation *on resize* */
  vkb::SwapchainBuilder swapchain_builder{ ren->device };
  auto swap_ret = swapchain_builder.build ();
  if (!swap_ret)
    {
      LOG_ERR ("failed to create swapchain: %s",
               swap_ret.error ().message ().c_str ());
      return NULL;
    }

  /* for deleting old swapchain later */
  /* vkb::destroy_swapchain (ren->swapchain); */

  ren->swapchain = swap_ret.value ();

  auto gq = ren->device.get_queue (vkb::QueueType::graphics);
  if (!gq.has_value ())
    {
      LOG_ERR ("failed to get graphics queue: %s",
               gq.error ().message ().c_str ());
      return NULL;
    }

  ren->graphics_queue = gq.value ();

  auto pq = ren->device.get_queue (vkb::QueueType::present);
  if (!pq.has_value ())
    {
      LOG_ERR ("failed to get present queue: %s",
               pq.error ().message ().c_str ());
      return NULL;
    }

  ren->present_queue = pq.value ();

  { /* create render pass */
    VkAttachmentDescription color_attachment = {};
    color_attachment.format = ren->swapchain.image_format;
    color_attachment.samples = VK_SAMPLE_COUNT_1_BIT;
    color_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    color_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    color_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    color_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    color_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    color_attachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentReference color_attachment_ref = {};
    color_attachment_ref.attachment = 0;
    color_attachment_ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass = {};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &color_attachment_ref;

    VkSubpassDependency dependency = {};
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;
    dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.srcAccessMask = 0;
    dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT
                               | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

    VkRenderPassCreateInfo render_pass_info = {};
    render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    render_pass_info.attachmentCount = 1;
    render_pass_info.pAttachments = &color_attachment;
    render_pass_info.subpassCount = 1;
    render_pass_info.pSubpasses = &subpass;
    render_pass_info.dependencyCount = 1;
    render_pass_info.pDependencies = &dependency;

    if (vkCreateRenderPass (ren->device.device, &render_pass_info, nullptr,
                            &ren->render_pass)
        != VK_SUCCESS)
      {
        LOG_ERR ("failed to create render pass");
        return NULL;
      }
  }

  { /* create framebuffers */
    ren->image_views = ren->swapchain.get_image_views ().value ();
    ren->framebuffers.resize (ren->image_views.size (), VK_NULL_HANDLE);

    for (size_t i = 0; i < ren->framebuffers.size (); i++)
      {
        VkImageView attachments[] = { ren->image_views[i] };

        VkFramebufferCreateInfo framebuffer_info = {};
        framebuffer_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebuffer_info.renderPass = ren->render_pass;
        framebuffer_info.attachmentCount = 1;
        framebuffer_info.pAttachments = attachments;
        framebuffer_info.width = ren->swapchain.extent.width;
        framebuffer_info.height = ren->swapchain.extent.height;
        framebuffer_info.layers = 1;

        if (vkCreateFramebuffer (ren->device.device, &framebuffer_info,
                                 nullptr, &ren->framebuffers[i])
            != VK_SUCCESS)
          {
            LOG_ERR ("failed to create framebuffer");
            return 0;
          }
      }
  }

  { /* create frame data */
    /* pipeline two frames at once */
    ren->frames.resize (2);

    VkDevice device = ren->device.device;

    VkCommandPoolCreateInfo pool_info = {};
    pool_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    pool_info.queueFamilyIndex
        = ren->device.get_queue_index (vkb::QueueType::graphics).value ();

    VkFenceCreateInfo fence_info = {};
    fence_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fence_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    VkSemaphoreCreateInfo semaphore_info = {};
    semaphore_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    for (auto &frame : ren->frames)
      {
        if (vkCreateCommandPool (device, &pool_info, NULL, &frame.command_pool)
            != VK_SUCCESS)
          {
            LOG_ERR ("failed to create command pool");
            return NULL;
          }

        if (vkCreateFence (device, &fence_info, NULL, &frame.is_in_flight)
            != VK_SUCCESS)
          {
            LOG_ERR ("failed to create is_in_flight fence");
            return NULL;
          }

        if (vkCreateSemaphore (device, &semaphore_info, NULL,
                               &frame.on_available)
            != VK_SUCCESS)
          {
            LOG_ERR ("failed to create on_available semaphore");
            return NULL;
          }

        if (vkCreateSemaphore (device, &semaphore_info, NULL,
                               &frame.on_finished)
            != VK_SUCCESS)
          {
            LOG_ERR ("failed to create on_finished semaphore");
            return NULL;
          }
      }
  }

  return ren;
}

void
vk_renderer_delete (vk_renderer_t *ren)
{
  VkDevice device = ren->device.device;

  vkDeviceWaitIdle (device);

  for (auto &frame : ren->frames)
    {
      if (frame.command_pool != VK_NULL_HANDLE)
        vkDestroyCommandPool (device, frame.command_pool, NULL);

      if (frame.is_in_flight != VK_NULL_HANDLE)
        vkDestroyFence (device, frame.is_in_flight, NULL);

      if (frame.on_available != VK_NULL_HANDLE)
        vkDestroySemaphore (device, frame.on_available, NULL);

      if (frame.on_finished != VK_NULL_HANDLE)
        vkDestroySemaphore (device, frame.on_finished, NULL);
    }

  for (auto fb : ren->framebuffers)
    vkDestroyFramebuffer (device, fb, NULL);

  if (ren->render_pass != VK_NULL_HANDLE)
    vkDestroyRenderPass (device, ren->render_pass, NULL);

  ren->swapchain.destroy_image_views (ren->image_views);
  vkb::destroy_swapchain (ren->swapchain);

  vkb::destroy_device (ren->device);
  vkb::destroy_surface (ren->instance, ren->surface);
  vkb::destroy_instance (ren->instance);

  delete ren;
}

void
vk_renderer_render_frame (vk_renderer_t *ren)
{
  ren->current_frame++;
  if (ren->current_frame >= ren->frames.size ())
    ren->current_frame = 0;

  auto &frame = ren->frames[ren->current_frame];

  vkWaitForFences (ren->device.device, 1, &frame.is_in_flight, VK_TRUE,
                   UINT64_MAX);

  uint32_t image_index = 0;
  if (vkAcquireNextImageKHR (ren->device.device, ren->swapchain.swapchain,
                             UINT64_MAX, frame.on_available, VK_NULL_HANDLE,
                             &image_index)
      != VK_SUCCESS)
    {
      LOG_ERR ("failed to acquire swapchain image");
    }

  vkResetCommandPool (ren->device.device, frame.command_pool, 0);

  VkCommandBuffer cmd;

  VkCommandBufferAllocateInfo alloc_info = {};
  alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  alloc_info.commandPool = frame.command_pool;
  alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  alloc_info.commandBufferCount = 1;

  if (vkAllocateCommandBuffers (ren->device.device, &alloc_info, &cmd)
      != VK_SUCCESS)
    {
      LOG_ERR ("failed to allocate command buffer");
      return;
    }

  if (cmd != VK_NULL_HANDLE)
    {
      VkCommandBufferBeginInfo begin_info = {};
      begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
      begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

      if (vkBeginCommandBuffer (cmd, &begin_info) != VK_SUCCESS)
        {
          LOG_ERR ("failed to begin command buffer");
          return;
        }

      VkRenderPassBeginInfo rp_info = {};
      rp_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
      rp_info.renderPass = ren->render_pass;
      rp_info.framebuffer = ren->framebuffers[image_index];
      rp_info.renderArea.offset = { 0, 0 };
      rp_info.renderArea.extent = ren->swapchain.extent;
      VkClearValue clearColor{ { { 0.0f, 0.0f, 0.0f, 1.0f } } };
      rp_info.clearValueCount = 1;
      rp_info.pClearValues = &clearColor;

      vkCmdBeginRenderPass (cmd, &rp_info, VK_SUBPASS_CONTENTS_INLINE);

      vkCmdEndRenderPass (cmd);

      if (vkEndCommandBuffer (cmd) != VK_SUCCESS)
        {
          LOG_ERR ("failed to end command buffer");
        }
    }

  { /* submit command buffer */
    VkSubmitInfo submit_info = {};
    submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    VkSemaphore wait_semaphores[] = { frame.on_available };
    VkPipelineStageFlags wait_stages[]
        = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
    submit_info.waitSemaphoreCount = 1;
    submit_info.pWaitSemaphores = wait_semaphores;
    submit_info.pWaitDstStageMask = wait_stages;

    submit_info.commandBufferCount = 1;
    submit_info.pCommandBuffers = &cmd;

    VkSemaphore signal_semaphores[] = { frame.on_finished };
    submit_info.signalSemaphoreCount = 1;
    submit_info.pSignalSemaphores = signal_semaphores;

    vkResetFences (ren->device.device, 1, &frame.is_in_flight);

    if (vkQueueSubmit (ren->graphics_queue, 1, &submit_info,
                       frame.is_in_flight)
        != VK_SUCCESS)
      {
        LOG_ERR ("failed to submit command buffer");
        return;
      }
  }

  { /* present to swapchain */
    VkPresentInfoKHR present_info = {};
    present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

    VkSemaphore signal_semaphores[] = { frame.on_finished };
    present_info.waitSemaphoreCount = 1;
    present_info.pWaitSemaphores = signal_semaphores;

    VkSwapchainKHR swapchains[] = { ren->swapchain.swapchain };
    present_info.swapchainCount = 1;
    present_info.pSwapchains = swapchains;

    present_info.pImageIndices = &image_index;

    if (vkQueuePresentKHR (ren->present_queue, &present_info) != VK_SUCCESS)
      {
        LOG_ERR ("failed to present swapchain image");
        return;
      }
  }
}
