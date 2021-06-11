#include "ui_script.h"

#include <stdio.h>
#include <wasmtime.h>

struct mdo_ui_script_s
{
  const mdo_allocator_t *alloc;
  mdo_result_t wasm_error;
  mdo_result_t wasmtime_error;
  mdo_result_t wasm_trap_error;

  wasm_engine_t *engine;
  wasm_store_t *store;

  wasmtime_linker_t *linker;
  wasm_module_t *module;
  wasm_instance_t *instance;

  wasm_functype_t *env_abort_functype;
  wasm_func_t *env_abort_func;
};

static mdo_result_t
log_wasmtime_error (mdo_ui_script_t *ui_script, wasmtime_error_t *error)
{
  wasm_byte_vec_t error_message;
  wasmtime_error_message (error, &error_message);
  wasmtime_error_delete (error);

  mdo_result_t result = ui_script->wasmtime_error;
  mdo_result_t logged = LOG_RESULT (result, error_message.data);
  wasm_byte_vec_delete (&error_message);

  return logged;
}

static mdo_result_t
log_wasm_trap (mdo_ui_script_t *ui_script, wasm_trap_t *trap)
{
  wasm_byte_vec_t trap_message;
  wasm_trap_message (trap, &trap_message);
  wasm_trap_delete (trap);

  mdo_result_t result = ui_script->wasm_trap_error;
  mdo_result_t logged = LOG_RESULT (result, trap_message.data);
  wasm_byte_vec_delete (&trap_message);

  return logged;
}

static wasm_trap_t *
env_abort_cb (void *env, const wasm_val_t args[], wasm_val_t results[])
{
  return NULL;
}

static void
finalizer_cb (void *env)
{
}

mdo_result_t
mdo_ui_script_create (mdo_ui_script_t **ui_script,
                      const mdo_allocator_t *alloc)
{
  mdo_ui_script_t *new_ui_script
      = mdo_allocator_malloc (alloc, sizeof (mdo_ui_script_t));
  *ui_script = new_ui_script;

  new_ui_script->alloc = alloc;
  new_ui_script->module = NULL;
  new_ui_script->instance = NULL;

  mdo_result_t wasm_error
      = mdo_result_create (MDO_LOG_ERROR, "wasm error: %s", 1, false);
  new_ui_script->wasm_error = wasm_error;

  mdo_result_t wasmtime_error
      = mdo_result_create (MDO_LOG_ERROR, "wasmtime error: %s", 1, false);
  new_ui_script->wasmtime_error = wasmtime_error;

  mdo_result_t wasm_trap_error
      = mdo_result_create (MDO_LOG_ERROR, "wasmtrap thrown: %s", 1, false);
  new_ui_script->wasm_trap_error = wasm_trap_error;

  new_ui_script->engine = wasm_engine_new ();
  if (!new_ui_script->engine)
    return LOG_RESULT (wasm_error, "failed to create engine");

  new_ui_script->store = wasm_store_new (new_ui_script->engine);
  if (!new_ui_script->store)
    return LOG_RESULT (wasm_error, "failed to create store");

  new_ui_script->linker = wasmtime_linker_new (new_ui_script->store);
  if (!new_ui_script->linker)
    return LOG_RESULT (wasmtime_error, "failed to create linker");

  wasm_name_t module_name = {
    .size = 3,
    .data = "env",
  };

  wasm_name_t func_name = {
    .size = 5,
    .data = "abort",
  };

  wasm_valtype_vec_t params;
  wasm_valtype_vec_new_uninitialized (&params, 4);

  for (size_t i = 0; i < params.size; i++)
    params.data[i] = wasm_valtype_new_i32 ();

  wasm_valtype_vec_t results;
  wasm_valtype_vec_new_empty (&results);

  new_ui_script->env_abort_functype = wasm_functype_new (&params, &results);

  new_ui_script->env_abort_func = wasm_func_new_with_env (
      new_ui_script->store, new_ui_script->env_abort_functype, env_abort_cb,
      new_ui_script, finalizer_cb);

  wasm_extern_t *func_extern
      = wasm_func_as_extern (new_ui_script->env_abort_func);

  wasmtime_error_t *error = wasmtime_linker_define (
      new_ui_script->linker, &module_name, &func_name, func_extern);
  if (error)
    return log_wasmtime_error (new_ui_script, error);

  return MDO_SUCCESS;
}

mdo_result_t
mdo_ui_script_load (mdo_ui_script_t *ui_script, const char *filename)
{
  const mdo_allocator_t *alloc = ui_script->alloc;
  mdo_result_t wasm_error = ui_script->wasm_error;

  FILE *f = fopen (filename, "rb");
  if (!f)
    return LOG_RESULT (wasm_error, "failed to open UI script file");

  wasm_byte_vec_t file_contents;

  fseek (f, 0, SEEK_END);
  file_contents.size = ftell (f);
  fseek (f, 0, SEEK_SET);
  file_contents.data = mdo_allocator_malloc (alloc, file_contents.size);
  fread (file_contents.data, 1, file_contents.size, f);
  fclose (f);

  wasmtime_error_t *wasmtime_error = wasmtime_module_new (
      ui_script->engine, &file_contents, &ui_script->module);
  mdo_allocator_free (alloc, file_contents.data);

  if (wasmtime_error)
    return log_wasmtime_error (ui_script, wasmtime_error);

  if (!ui_script->module)
    return LOG_RESULT (wasm_error, "failed to compile UI script");

  wasm_trap_t *trap = NULL;
  wasmtime_error = wasmtime_linker_instantiate (
      ui_script->linker, ui_script->module, &ui_script->instance, &trap);

  if (wasmtime_error)
    return log_wasmtime_error (ui_script, wasmtime_error);

  if (trap)
    return log_wasm_trap (ui_script, trap);

  if (!ui_script->instance)
    return LOG_RESULT (wasm_error, "failed to instantiate module");

  return MDO_SUCCESS;
}

void
mdo_ui_script_delete (mdo_ui_script_t *ui_script)
{

  const mdo_allocator_t *alloc = ui_script->alloc;

  if (ui_script->env_abort_func)
    wasm_func_delete (ui_script->env_abort_func);

  if (ui_script->env_abort_functype)
    wasm_functype_delete (ui_script->env_abort_functype);

  if (ui_script->instance)
    wasm_instance_delete (ui_script->instance);

  if (ui_script->module)
    wasm_module_delete (ui_script->module);

  if (ui_script->linker)
    wasmtime_linker_delete (ui_script->linker);

  if (ui_script->store)
    wasm_store_delete (ui_script->store);

  if (ui_script->engine)
    wasm_engine_delete (ui_script->engine);

  mdo_allocator_free (alloc, ui_script);
}
