/** @file api.h
 */

#include <wasmtime.h>

#include "script.h"

/**
 * Helper macro to both declare and define scripting API callbacks.
 */
#define SCRIPT_CALLBACK(name)                                                 \
  wasm_trap_t *name (void *env, wasmtime_caller_t *caller,                    \
                     const wasmtime_val_t *args, size_t arg_num,              \
                     wasmtime_val_t *results, size_t result_num)
