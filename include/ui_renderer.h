/** @file ui_renderer.h
 */

#pragma once

#include <mdo-utils/allocator.h>
#include <mdo-utils/result.h>

/** @typedef mdo_ui_renderer_t
 */
typedef struct mdo_ui_renderer_s mdo_ui_renderer_t;

/** @typedef mdo_ui_renderer_params_t
 */
typedef struct mdo_ui_renderer_params_s
{
  /** @function
   * @param userdata
   */
  void (*finalize) (void *);
} mdo_ui_renderer_params_t;

/** @function mdo_ui_renderer_create
 * @param ui_renderer
 * @param alloc
 * @param params
 * @param userdata
 * @return #mdo_result_t.
 */
mdo_result_t mdo_ui_renderer_create (mdo_ui_renderer_t **,
                                     const mdo_allocator_t *,
                                     mdo_ui_renderer_params_t *, void *);

/** @function mdo_ui_renderer_delete
 * @param ui_renderer
 */
void mdo_ui_renderer_delete (mdo_ui_renderer_t *);
