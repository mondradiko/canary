/** @file gles_renderer.h
 */

#pragma once

#include "panel.h"

/** @typedef gles_renderer_t
 */
typedef struct gles_renderer_s gles_renderer_t;

/** @function gles_renderer_create
 * @param new_ren
 * @return Zero on success.
 */
int gles_renderer_create (gles_renderer_t **, canary_panel_t *);

/** @function gles_renderer_delete
 * @param ren
 */
void gles_renderer_delete (gles_renderer_t *);
