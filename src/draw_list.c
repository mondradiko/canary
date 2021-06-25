/** @file draw_list.c
 */

#include "draw_list.h"

#include <string.h> /* for memcpy */

struct canary_draw_list_s
{
  const mdo_allocator_t *alloc;

  /* TODO(marceline-cramer): mdo-utils vector */
  struct
  {
    canary_draw_vertex_t *vals;
    size_t size;
    size_t capacity;
  } vertices;

  struct
  {
    canary_draw_index_t *vals;
    size_t size;
    size_t capacity;
  } indices;
};

mdo_result_t
canary_draw_list_create (canary_draw_list_t **draw_list,
                         const mdo_allocator_t *alloc)
{
  canary_draw_list_t *new_draw_list
      = mdo_allocator_malloc (alloc, sizeof (canary_draw_list_t));
  *draw_list = new_draw_list;

  new_draw_list->alloc = alloc;

  new_draw_list->vertices.vals = NULL;
  new_draw_list->vertices.size = 0;
  new_draw_list->vertices.capacity = 0;

  new_draw_list->indices.vals = NULL;
  new_draw_list->indices.size = 0;
  new_draw_list->indices.capacity = 0;

  return MDO_SUCCESS;
}

void
canary_draw_list_delete (canary_draw_list_t *draw_list)
{
  const mdo_allocator_t *alloc = draw_list->alloc;

  if (draw_list->vertices.vals)
    mdo_allocator_free (alloc, draw_list->vertices.vals);

  if (draw_list->indices.vals)
    mdo_allocator_free (alloc, draw_list->indices.vals);

  mdo_allocator_free (alloc, draw_list);
}

void
canary_draw_list_clear (canary_draw_list_t *draw_list)
{
  draw_list->vertices.size = 0;
  draw_list->indices.size = 0;
}

canary_draw_index_t
canary_draw_vertex (canary_draw_list_t *draw_list,
                    const canary_draw_vertex_t *vertex)
{
  const mdo_allocator_t *alloc = draw_list->alloc;

  canary_draw_index_t index = draw_list->vertices.size++;

  if (draw_list->vertices.capacity == 0)
    {
      draw_list->vertices.capacity = 1024;
      draw_list->vertices.vals = mdo_allocator_calloc (
          alloc, draw_list->vertices.capacity, sizeof (canary_draw_vertex_t));
    }
  else if (index >= draw_list->vertices.capacity)
    {
      draw_list->vertices.capacity = draw_list->vertices.capacity << 1;
      draw_list->vertices.vals = mdo_allocator_realloc (
          alloc, draw_list->vertices.vals,
          sizeof (canary_draw_vertex_t) * draw_list->vertices.capacity);
    }

  canary_draw_vertex_t *dst = &draw_list->vertices.vals[index];
  memcpy (dst, vertex, sizeof (canary_draw_vertex_t));

  return index;
}

size_t
canary_draw_list_vertex_count (canary_draw_list_t *draw_list)
{
  return draw_list->vertices.size;
}

canary_draw_vertex_t *
canary_draw_list_vertex_buffer (canary_draw_list_t *draw_list)
{
  return draw_list->vertices.vals;
}

size_t
canary_draw_list_index_count (canary_draw_list_t *draw_list)
{
  return draw_list->indices.size;
}

canary_draw_index_t *
canary_draw_list_index_buffer (canary_draw_list_t *draw_list)
{
  return draw_list->indices.vals;
}

void
canary_draw_triangle (canary_draw_list_t *draw_list,
                      canary_draw_index_t vertex1, canary_draw_index_t vertex2,
                      canary_draw_index_t vertex3)
{
  const mdo_allocator_t *alloc = draw_list->alloc;

  size_t index_offset = draw_list->indices.size;
  draw_list->indices.size += 3;

  if (draw_list->indices.capacity == 0)
    {
      draw_list->indices.capacity = 1536;
      draw_list->indices.vals = mdo_allocator_calloc (
          alloc, draw_list->indices.capacity, sizeof (canary_draw_index_t));
    }
  else if (draw_list->indices.size > draw_list->indices.capacity)
    {
      draw_list->indices.capacity = draw_list->indices.capacity << 1;
      draw_list->indices.vals = mdo_allocator_realloc (
          alloc, draw_list->indices.vals,
          sizeof (canary_draw_index_t) * draw_list->indices.capacity);
    }

  canary_draw_index_t *indices = &draw_list->indices.vals[index_offset];

  indices[0] = vertex1;
  indices[1] = vertex2;
  indices[2] = vertex3;
}
