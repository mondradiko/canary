/** @file script.h
 */

#pragma once

#include <stdint.h> /* for uint32_t */

#include <mdo-utils/allocator.h>
#include <mdo-utils/result.h>

#include "panel.h"

/** @typedef canary_script_t
 */
typedef struct canary_script_s canary_script_t;

/** @typedef canary_panel_key_t
 */
typedef uint32_t canary_panel_key_t;

/** @typedef canary_input_event_t
 */
typedef enum
{
  CANARY_HOVER,
  CANARY_SELECT,
  CANARY_DRAG,
  CANARY_DESELECT,
} canary_input_event_t;

/** @function canary_script_create
 * @param script
 * @param alloc
 * @return #mdo_result_t.
 */
mdo_result_t canary_script_create (canary_script_t **,
                                   const mdo_allocator_t *);

/** @function canary_script_delete
 * @param script
 */
void canary_script_delete (canary_script_t *);

/** @function canary_script_load
 * @param script
 * @param filename
 * @return #mdo_result_t.
 */
mdo_result_t canary_script_load (canary_script_t *, const char *);

/** @function canary_script_new_trap
 * @param script
 * @param message
 * @return #wasm_trap_t.
 */
wasm_trap_t *canary_script_new_trap (canary_script_t *, const char *);

/** @function canary_script_update
 * @param script
 * @param dt
 */
void canary_script_update (canary_script_t *, float);

/** @function canary_script_bind_panel
 * @param script
 * @param panel
 * @param panel_key
 * @return #mdo_result_t.
 */
mdo_result_t canary_script_bind_panel (canary_script_t *, canary_panel_t *,
                                       canary_panel_key_t *);

/** @function canary_script_unbind_panel
 * @param script
 * @param panel_key
 */
void canary_script_unbind_panel (canary_script_t *, canary_panel_key_t);

/** @function canary_script_lookup_panel
 * @param script
 * @param panel_key
 * @return A pointer to the panel, or NULL on error.
 */
canary_panel_t *canary_script_lookup_panel (canary_script_t *,
                                            canary_panel_key_t);

/** @function canary_script_on_input
 * @param script
 * @param panel_key
 * @param event_type
 * @param coords
 */
void canary_script_on_input (canary_script_t *, canary_panel_key_t,
                             canary_input_event_t, float[2]);
