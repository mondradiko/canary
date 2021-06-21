/** @file draw_list.c
 */

#include "draw_list.h"

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
  mdo_ui_draw_list_t *new_ui_draw
      = mdo_allocator_malloc (alloc, sizeof (mdo_ui_draw_list_t));
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
  const mdo_allocator_t *alloc = ui_draw->alloc;

  mdo_ui_draw_index_t index = ui_draw->vertices.size++;

  if (ui_draw->vertices.capacity == 0)
    {
      ui_draw->vertices.capacity = 1024;
      ui_draw->vertices.vals = mdo_allocator_calloc (
          alloc, ui_draw->vertices.capacity, sizeof (mdo_ui_draw_vertex_t));
    }
  else if (index >= ui_draw->vertices.capacity)
    {
      ui_draw->vertices.capacity = ui_draw->vertices.capacity << 1;
      ui_draw->vertices.vals = mdo_allocator_realloc (
          alloc, ui_draw->vertices.vals,
          sizeof (mdo_ui_draw_vertex_t) * ui_draw->vertices.capacity);
    }

  return index;
}

void
mdo_ui_draw_triangle (mdo_ui_draw_list_t *ui_draw, mdo_ui_draw_index_t vertex1,
                      mdo_ui_draw_index_t vertex2, mdo_ui_draw_index_t vertex3)
{
  const mdo_allocator_t *alloc = ui_draw->alloc;

  size_t index_offset = ui_draw->indices.size;
  ui_draw->indices.size += 3;

  if (ui_draw->indices.capacity == 0)
    {
      ui_draw->indices.capacity = 1536;
      ui_draw->indices.vals = mdo_allocator_calloc (
          alloc, ui_draw->indices.capacity, sizeof (mdo_ui_draw_index_t));
    }
  else if (ui_draw->indices.size > ui_draw->indices.capacity)
    {
      ui_draw->indices.capacity = ui_draw->indices.capacity << 1;
      ui_draw->indices.vals = mdo_allocator_realloc (
          alloc, ui_draw->indices.vals,
          sizeof (mdo_ui_draw_index_t) * ui_draw->indices.capacity);
    }

  mdo_ui_draw_index_t *indices = &ui_draw->indices.vals[index_offset];

  indices[0] = vertex1;
  indices[1] = vertex2;
  indices[2] = vertex3;
}
