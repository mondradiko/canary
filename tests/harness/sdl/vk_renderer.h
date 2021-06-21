#pragma once

#include <SDL2/SDL.h>

#include <mdo-utils/allocator.h>

#include "panel.h"

/** @typedef vk_renderer_t
 */
typedef struct vk_renderer_s vk_renderer_t;

vk_renderer_t *vk_renderer_create (const mdo_allocator_t *, canary_panel_t *,
                                   SDL_Window *);

void vk_renderer_delete (vk_renderer_t *);

void vk_renderer_render_frame (vk_renderer_t *);
