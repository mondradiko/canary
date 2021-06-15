/** @file ui_draw_list.c
 */

#include "ui_draw_list.h"

mdo_result_t
mdo_ui_draw_list_create (mdo_ui_draw_list_t **ui_draw,
                         const mdo_allocator_t *alloc)
{
  return MDO_SUCCESS;
}

void
mdo_ui_draw_list_delete (mdo_ui_draw_list_t *ui_draw)
{
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
