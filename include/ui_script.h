/** @file ui_script.h
 */

#pragma once

#include <stdint.h> /* for uint32_t */

#include <mdo-utils/allocator.h>
#include <mdo-utils/result.h>

/** @typedef mdo_ui_script_t
 */
typedef struct mdo_ui_script_s mdo_ui_script_t;

/** @typedef mdo_ui_panel_key_t
 */
typedef uint32_t mdo_ui_panel_key_t;

/** @function mdo_ui_script_create
 * @param ui_script
 * @param alloc
 * @return #mdo_result_t.
 */
mdo_result_t mdo_ui_script_create (mdo_ui_script_t **,
                                   const mdo_allocator_t *);

/** @function mdo_ui_script_delete
 * @param ui_script
 */
void mdo_ui_script_delete (mdo_ui_script_t *);

/** @function mdo_ui_script_load
 * @param ui_script
 * @param filename
 * @return #mdo_result_t.
 */
mdo_result_t mdo_ui_script_load (mdo_ui_script_t *, const char *);

/** @function mdo_ui_script_bind_panel
 */

/** @function mdo_ui_script_lookup_panel
 */
