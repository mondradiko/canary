/** @file panel.h
 */

#pragma once

#include <mdo-utils/allocator.h>
#include <mdo-utils/result.h>
#include <wasmtime.h> /* for script API */

#include "draw_list.h"

/** @typedef canary_panel_t
 */
typedef struct canary_panel_s canary_panel_t;

/** @function canary_panel_create
 */
mdo_result_t canary_panel_create (canary_panel_t **, const mdo_allocator_t *);

/** @function canary_panel_delete
 */
void canary_panel_delete (canary_panel_t *);

/** @function canary_panel_set_color
 */
void canary_panel_set_color (canary_panel_t *, const float[4]);

/** @function canary_panel_get_color
 */
void canary_panel_get_color (canary_panel_t *, float[4]);

/** @function canary_panel_set_size
 * @param panel
 * @param size
 */
void canary_panel_set_size (canary_panel_t *, const float[2]);

/** @function canary_panel_get_size
 * @param panel
 * @param size
 */
void canary_panel_get_size (canary_panel_t *, float[2]);

/** @function canary_panel_set_draw_list
 * @param panel
 * @param ui_draw
 */
void canary_panel_set_draw_list (canary_panel_t *, canary_draw_list_t *);

/** @function canary_panel_get_draw_list
 * @param panel
 * @return #canary_draw_list_t.
 */
canary_draw_list_t *canary_panel_get_draw_list (canary_panel_t *);

