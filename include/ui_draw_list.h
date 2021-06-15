/** @file ui_draw_list.h
 */

#pragma once

#include <stdint.h> /* for uint32_t */

#include <mdo-utils/allocator.h>
#include <mdo-utils/result.h>

/** @typedef mdo_ui_draw_list_t
 */
typedef struct mdo_ui_draw_list_s mdo_ui_draw_list_t;

/** @typedef mdo_ui_draw_vertex_t
 */
typedef struct mdo_ui_draw_vertex_s
{
  float position[2];
  float color[4];
} mdo_ui_draw_vertex_t;

/** @typedef mdo_ui_draw_index_t
 */
typedef uint32_t mdo_ui_draw_index_t;

/** @typedef mdo_ui_draw_list_create
 * @param ui_draw
 * @param alloc
 * @return #mdo_result_t.
 */
mdo_result_t mdo_ui_draw_list_create (mdo_ui_draw_list_t **,
                                      const mdo_allocator_t *);

/** @function mdo_ui_draw_list_delete
 * @param ui_draw
 */
void mdo_ui_draw_list_delete (mdo_ui_draw_list_t *);

/** @function mdo_ui_draw_list_concat
 * Concatenates two #mdo_ui_draw_list.
 * @param ui_draw_a The first draw list. Receives concatenated data.
 * @param ui_draw_b The second draw list.
 */
void mdo_ui_draw_list_concat (mdo_ui_draw_list_t *,
                              const mdo_ui_draw_list_t *);

/** @function mdo_ui_draw_list_clear
 * @param ui_draw
 */
void mdo_ui_draw_list_clear (mdo_ui_draw_list_t *);

/** @function mdo_ui_draw_vertex
 * @return #mdo_ui_draw_index_t.
 */
mdo_ui_draw_index_t mdo_ui_draw_vertex (const mdo_ui_draw_vertex_t *);

/** @function mdo_ui_draw_triangle
 * @param ui_draw
 * @param vertex1
 * @param vertex2
 * @param vertex3
 */
void mdo_ui_draw_triangle (mdo_ui_draw_list_t *, mdo_ui_draw_index_t,
                           mdo_ui_draw_index_t, mdo_ui_draw_index_t);
