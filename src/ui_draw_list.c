/** @file ui_draw_list.c
 */

#include "ui_draw_list.h"

struct mdo_ui_draw_list_s
{
  const mdo_allocator_t *alloc;

  /* TODO(marceline-cramer): mdo-utils vector */
  struct
  {
    mdo_ui_draw_vertex_t *vals;
    size_t size;
    size_t capacity;
  } vertices;

  struct
  {
    mdo_ui_draw_index_t *vals;
    size_t size;
    size_t capacity;
  } indices;
};

mdo_result_t
mdo_ui_draw_list_create (mdo_ui_draw_list_t **ui_draw,
                         const mdo_allocator_t *alloc)
{
  mdo_ui_draw_list_t *new_ui_draw = mdo_allocator_malloc (alloc, sizeof (mdo_ui_draw_list_t));
  *ui_draw = new_ui_draw;

  new_ui_draw->alloc = alloc;

  new_ui_draw->vertices.vals = NULL;
  new_ui_draw->vertices.size = 0;
  new_ui_draw->vertices.capacity = 0;

  new_ui_draw->indices.vals = NULL;
  new_ui_draw->indices.size = 0;
  new_ui_draw->indices.capacity = 0;

  return MDO_SUCCESS;
}

void
mdo_ui_draw_list_delete (mdo_ui_draw_list_t *ui_draw)
{
  const mdo_allocator_t *alloc = ui_draw->alloc;

  if (ui_draw->vertices.vals)
    mdo_allocator_free (alloc, ui_draw->vertices.vals);

  if (ui_draw->indices.vals)
    mdo_allocator_free (alloc, ui_draw->indices.vals);

  mdo_allocator_free (alloc, ui_draw);
}

mdo_ui_draw_index_t
mdo_ui_draw_vertex (mdo_ui_draw_list_t *ui_draw,
                    const mdo_ui_draw_vertex_t *vertex)
{
  return 0;
}

void
mdo_ui_draw_triangle (mdo_ui_draw_list_t *ui_draw, mdo_ui_draw_index_t vertex1,
                      mdo_ui_draw_index_t vertex2, mdo_ui_draw_index_t vertex3)
{
}
