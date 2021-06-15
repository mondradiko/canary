#pragma once

#include <mdo-utils/allocator.h>

/** @typedef vk_renderer_t
 */
typedef struct vk_renderer_s vk_renderer_t;

vk_renderer_t *vk_renderer_create (const mdo_allocator_t *);

void vk_renderer_delete (vk_renderer_t *);
