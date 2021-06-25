/** @file gles_renderer.c
 */

#include "gles_renderer.h"

#include <stdlib.h>

struct gles_renderer_s
{
  canary_panel_t *panel;
};

int
gles_renderer_create (gles_renderer_t **new_ren, canary_panel_t *panel)
{
  gles_renderer_t *ren = malloc (sizeof (gles_renderer_t));
  *new_ren = ren;

  ren->panel = panel;

  return 0;
}

void
gles_renderer_delete (gles_renderer_t *ren)
{
  free (ren);
}
