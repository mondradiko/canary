/** @file ui_ctx.h
 */

#pragma once

#include <mdo-utils/allocator.h>
#include <mdo-utils/result.h>

/** @typedef mdo_ui_ctx_t
 */
typedef struct mdo_ui_ctx_s mdo_ui_ctx_t;

/** @function mdo_ui_ctx_create
 * @param ui_ctx
 * @param alloc
 * @return #mdo_result_t.
 */
mdo_result_t mdo_ui_ctx_create (mdo_ui_ctx_t **, const mdo_allocator_t *);

/** @function mdo_ui_ctx_delete
 * @return #mdo_result_t.
 */
void mdo_ui_ctx_delete (mdo_ui_ctx_t *);

/** @function mdo_ui_ctx_set_renderer
 */

/** @function mdo_ui_ctx_update
 */

/** @function mdo_ui_ctx_render
 */
