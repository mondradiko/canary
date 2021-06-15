/** @file ui_panel.c
 */

#include "ui_panel.h"

#include <string.h> /* for memcpy */

#include "ui_script.h"

struct mdo_ui_panel_s
{
  const mdo_allocator_t *alloc;

  float color[4];

  mdo_ui_draw_list_t *draw_list;
};

mdo_result_t
mdo_ui_panel_create (mdo_ui_panel_t **ui_panel, const mdo_allocator_t *alloc)
{
  mdo_ui_panel_t *new_ui_panel
      = mdo_allocator_malloc (alloc, sizeof (mdo_ui_panel_t));
  *ui_panel = new_ui_panel;

  new_ui_panel->alloc = alloc;
  new_ui_panel->draw_list = NULL;

  return MDO_SUCCESS;
}

void
mdo_ui_panel_delete (mdo_ui_panel_t *ui_panel)
{
  const mdo_allocator_t *alloc = ui_panel->alloc;

  mdo_allocator_free (alloc, ui_panel);
}

void
mdo_ui_panel_set_color (mdo_ui_panel_t *ui_panel, const float color[4])
{
  memcpy (ui_panel->color, color, sizeof (float) * 4);
}

void
mdo_ui_panel_get_color (mdo_ui_panel_t *ui_panel, float color[4])
{
  memcpy (color, ui_panel->color, sizeof (float) * 4);
}

void
mdo_ui_panel_set_draw_list (mdo_ui_panel_t *ui_panel,
                            mdo_ui_draw_list_t *ui_draw)
{
  ui_panel->draw_list = ui_draw;
}

mdo_ui_draw_list_t *
mdo_ui_panel_get_draw_list (mdo_ui_panel_t *ui_panel)
{
  return ui_panel->draw_list;
}

static wasm_trap_t *
get_panel (mdo_ui_script_t *ui_script, const wasm_val_t *self,
           mdo_ui_panel_t **ui_panel)
{
  if (self->kind != WASM_I32)
    {
      return mdo_ui_script_new_trap (ui_script, "self is not an i32");
    }

  *ui_panel = mdo_ui_script_lookup_panel (ui_script, self->of.i32);

  if (!*ui_panel)
    {
      return mdo_ui_script_new_trap (ui_script, "failed to look up panel");
    }

  return NULL;
}

wasm_trap_t *
mdo_ui_panel_set_color_cb (void *env, const wasm_val_vec_t *args,
                           wasm_val_vec_t *results)
{
  mdo_ui_panel_t *ui_panel;
  wasm_trap_t *trap = get_panel (env, &args->data[0], &ui_panel);

  if (!trap)
    {
      float color[4] = {
        args->data[1].of.f32,
        args->data[2].of.f32,
        args->data[3].of.f32,
        args->data[4].of.f32,
      };

      mdo_ui_panel_set_color (ui_panel, color);
    }

  return trap;
}

static wasm_trap_t *
get_draw_list (mdo_ui_script_t *ui_script, const wasm_val_t *panel,
               mdo_ui_draw_list_t **ui_draw)
{
  mdo_ui_panel_t *ui_panel;
  wasm_trap_t *trap = get_panel (ui_script, panel, &ui_panel);

  if (trap)
    return trap;

  *ui_draw = mdo_ui_panel_get_draw_list (ui_panel);

  if (!*ui_draw)
    {
      return mdo_ui_script_new_trap (ui_script,
                                     "panel has no current draw list");
    }

  return NULL;
}

static mdo_ui_draw_vertex_t
make_vertex (const wasm_val_t *coord_args, float color[4])
{
  mdo_ui_draw_vertex_t vertex;
  vertex.position[0] = coord_args[0].of.f32;
  vertex.position[1] = coord_args[1].of.f32;
  memcpy (vertex.color, color, sizeof (float) * 4);
  return vertex;
}

wasm_trap_t *
mdo_ui_panel_draw_triangle_cb (void *env, const wasm_val_vec_t *args,
                               wasm_val_vec_t *results)
{
  mdo_ui_draw_list_t *ui_draw;
  wasm_trap_t *trap = get_draw_list (env, &args->data[0], &ui_draw);

  if (trap)
    return trap;

  float color[4];
  color[0] = args->data[7].of.f32;
  color[1] = args->data[8].of.f32;
  color[2] = args->data[9].of.f32;
  color[3] = args->data[10].of.f32;

  mdo_ui_draw_vertex_t vertex1 = make_vertex (&args->data[1], color);
  mdo_ui_draw_vertex_t vertex2 = make_vertex (&args->data[3], color);
  mdo_ui_draw_vertex_t vertex3 = make_vertex (&args->data[5], color);

  mdo_ui_draw_index_t index1 = mdo_ui_draw_vertex (ui_draw, &vertex1);
  mdo_ui_draw_index_t index2 = mdo_ui_draw_vertex (ui_draw, &vertex2);
  mdo_ui_draw_index_t index3 = mdo_ui_draw_vertex (ui_draw, &vertex3);

  mdo_ui_draw_triangle (ui_draw, index1, index2, index3);

  return NULL;
}
