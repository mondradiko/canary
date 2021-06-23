/** @file panel.c
 */

#include "panel.h"

#include <string.h> /* for memcpy */

#include "api.h"

struct canary_panel_s
{
  const mdo_allocator_t *alloc;

  float color[4];
  float size[2];

  canary_draw_list_t *draw_list;
};

mdo_result_t
canary_panel_create (canary_panel_t **panel, const mdo_allocator_t *alloc)
{
  canary_panel_t *new_panel
      = mdo_allocator_malloc (alloc, sizeof (canary_panel_t));
  *panel = new_panel;

  new_panel->alloc = alloc;
  new_panel->draw_list = NULL;

  return MDO_SUCCESS;
}

void
canary_panel_delete (canary_panel_t *panel)
{
  const mdo_allocator_t *alloc = panel->alloc;

  mdo_allocator_free (alloc, panel);
}

void
canary_panel_set_color (canary_panel_t *panel, const float color[4])
{
  memcpy (panel->color, color, sizeof (float) * 4);
}

void
canary_panel_get_color (canary_panel_t *panel, float color[4])
{
  memcpy (color, panel->color, sizeof (float) * 4);
}

void
canary_panel_set_size (canary_panel_t *panel, const float size[2])
{
  memcpy (panel->size, size, sizeof (float) * 2);
}

void
canary_panel_get_size (canary_panel_t *panel, float size[2])
{
  memcpy (size, panel->size, sizeof (float) * 2);
}

void
canary_panel_set_draw_list (canary_panel_t *panel,
                            canary_draw_list_t *draw_list)
{
  panel->draw_list = draw_list;
}

canary_draw_list_t *
canary_panel_get_draw_list (canary_panel_t *panel)
{
  return panel->draw_list;
}

static wasm_trap_t *
get_panel (canary_script_t *script, const wasmtime_val_t *self,
           canary_panel_t **panel)
{
  if (self->kind != WASMTIME_I32)
    {
      return canary_script_new_trap (script, "self is not an i32");
    }

  *panel = canary_script_lookup_panel (script, self->of.i32);

  if (!*panel)
    {
      return canary_script_new_trap (script, "failed to look up panel");
    }

  return NULL;
}

SCRIPT_CALLBACK (canary_panel_get_width_cb)
{
  canary_panel_t *panel;
  wasm_trap_t *trap = get_panel (env, args, &panel);

  if (!trap)
    {
      float size[2];
      canary_panel_get_size (panel, size);
      results[0].of.f32 = size[0];
    }

  return trap;
}

SCRIPT_CALLBACK (canary_panel_get_height_cb)
{
  canary_panel_t *panel;
  wasm_trap_t *trap = get_panel (env, args, &panel);

  if (!trap)
    {
      float size[2];
      canary_panel_get_size (panel, size);
      results[0].of.f32 = size[1];
    }

  return trap;
}

SCRIPT_CALLBACK (canary_panel_set_size_cb)
{
  canary_panel_t *panel;
  wasm_trap_t *trap = get_panel (env, args, &panel);

  if (!trap)
    {
      float size[2] = {
        args[1].of.f32,
        args[2].of.f32,
      };

      canary_panel_set_size (panel, size);
    }

  return trap;
}

SCRIPT_CALLBACK (canary_panel_set_color_cb)
{
  canary_panel_t *panel;
  wasm_trap_t *trap = get_panel (env, args, &panel);

  if (!trap)
    {
      float color[4] = {
        args[1].of.f32,
        args[2].of.f32,
        args[3].of.f32,
        args[4].of.f32,
      };

      canary_panel_set_color (panel, color);
    }

  return trap;
}

static wasm_trap_t *
get_draw_list (canary_script_t *script, const wasmtime_val_t *self,
               canary_draw_list_t **draw_list)
{
  canary_panel_t *panel;
  wasm_trap_t *trap = get_panel (script, self, &panel);

  if (trap)
    return trap;

  *draw_list = canary_panel_get_draw_list (panel);

  if (!*draw_list)
    {
      return canary_script_new_trap (script, "panel has no current draw list");
    }

  return NULL;
}

static canary_draw_vertex_t
make_vertex (const wasmtime_val_t *coord_args, float color[4])
{
  canary_draw_vertex_t vertex;
  vertex.position[0] = coord_args[0].of.f32;
  vertex.position[1] = coord_args[1].of.f32;
  memcpy (vertex.color, color, sizeof (float) * 4);
  return vertex;
}

SCRIPT_CALLBACK (canary_panel_draw_triangle_cb)
{
  canary_draw_list_t *draw_list;
  wasm_trap_t *trap = get_draw_list (env, args, &draw_list);

  if (trap)
    return trap;

  float color[4];
  color[0] = args[7].of.f32;
  color[1] = args[8].of.f32;
  color[2] = args[9].of.f32;
  color[3] = args[10].of.f32;

  canary_draw_vertex_t vertex1 = make_vertex (&args[1], color);
  canary_draw_vertex_t vertex2 = make_vertex (&args[3], color);
  canary_draw_vertex_t vertex3 = make_vertex (&args[5], color);

  canary_draw_index_t index1 = canary_draw_vertex (draw_list, &vertex1);
  canary_draw_index_t index2 = canary_draw_vertex (draw_list, &vertex2);
  canary_draw_index_t index3 = canary_draw_vertex (draw_list, &vertex3);

  canary_draw_triangle (draw_list, index1, index2, index3);

  return NULL;
}
