/** @file ui_ctx.c
 */

#include "ui_ctx.h"

struct mdo_ui_ctx_s
{
  const mdo_allocator_t *alloc;
};

mdo_result_t
mdo_ui_ctx_create (mdo_ui_ctx_t **ui_ctx, const mdo_allocator_t *alloc)
{
  mdo_ui_ctx_t *new_ui_ctx
      = mdo_allocator_malloc (alloc, sizeof (mdo_ui_ctx_t));
  *ui_ctx = new_ui_ctx;

  new_ui_ctx->alloc = alloc;

  return MDO_SUCCESS;
}

void
mdo_ui_ctx_delete (mdo_ui_ctx_t *ui_ctx)
{
  const mdo_allocator_t *alloc = ui_ctx->alloc;

  mdo_allocator_free (alloc, ui_ctx);
}
