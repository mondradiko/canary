/** @file ui_panel.c
 */

#include "ui_panel.h"

struct mdo_ui_panel_s
{
  const mdo_allocator_t *alloc;
};

mdo_result_t
mdo_ui_panel_create (mdo_ui_panel_t **ui_panel, const mdo_allocator_t *alloc)
{
  mdo_ui_panel_t *new_ui_panel
      = mdo_allocator_malloc (alloc, sizeof (mdo_ui_panel_t));
  *ui_panel = new_ui_panel;

  new_ui_panel->alloc = alloc;

  return MDO_SUCCESS;
}

void
mdo_ui_panel_delete (mdo_ui_panel_t *ui_panel)
{
  const mdo_allocator_t *alloc = ui_panel->alloc;

  mdo_allocator_free (alloc, ui_panel);
}
