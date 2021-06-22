/** @file draw_list.h
 */

#pragma once

#include <stdint.h> /* for uint32_t */

#include <mdo-utils/allocator.h>
#include <mdo-utils/result.h>

/** @typedef canary_draw_list_t
 */
typedef struct canary_draw_list_s canary_draw_list_t;

/** @typedef canary_draw_vertex_t
 */
typedef struct canary_draw_vertex_s
{
  float position[2];
  float color[4];
} canary_draw_vertex_t;

/** @typedef canary_draw_index_t
 */
typedef uint32_t canary_draw_index_t;

/** @typedef canary_draw_list_create
 * @param scriptdraw
 * @param alloc
 * @return #mdo_result_t.
 */
mdo_result_t canary_draw_list_create (canary_draw_list_t **,
                                      const mdo_allocator_t *);

/** @function canary_draw_list_delete
 * @param ui_draw
 */
void canary_draw_list_delete (canary_draw_list_t *);

/** @function canary_draw_list_concat
 * Concatenates two #canary_draw_list.
 * @param ui_draw_a The first draw list. Receives concatenated data.
 * @param ui_draw_b The second draw list.
 */
void canary_draw_list_concat (canary_draw_list_t *,
                              const canary_draw_list_t *);

/** @function canary_draw_list_clear
 * @param ui_draw
 */
void canary_draw_list_clear (canary_draw_list_t *);

/** @function canary_draw_vertex
 * @param ui_draw
 * @param vertex #canary_draw_vertex_t.
 * @return #canary_draw_index_t.
 */
canary_draw_index_t canary_draw_vertex (canary_draw_list_t *,
                                        const canary_draw_vertex_t *);

/** @function canary_draw_list_vertex_count
 * @param ui_draw
 * @return The number of vertices in the list.
 */
size_t canary_draw_list_vertex_count (canary_draw_list_t *);

/** @function canary_draw_list_vertex_buffer
 * @param ui_draw
 * @return A pointer to the vertex buffer in the list.
 */
canary_draw_vertex_t *canary_draw_vertex_buffer (canary_draw_list_t *);

/** @function canary_draw_list_index_count
 * @param ui_draw
 * @return The number of indices in the list.
 */
size_t canary_draw_list_index_count (canary_draw_list_t *);

/** @function canary_draw_list_index_buffer
 * @param ui_draw
 * @return A pointer to the index buffer in the list.
 */
canary_draw_index_t *canary_draw_list_index_buffer (canary_draw_list_t *);

/** @function canary_draw_triangle
 * @param ui_draw
 * @param vertex1
 * @param vertex2
 * @param vertex3
 */
void canary_draw_triangle (canary_draw_list_t *, canary_draw_index_t,
                           canary_draw_index_t, canary_draw_index_t);
