/** @file test_ui_ctx.c
 */

#include "test_common.h"
#include "ui_ctx.h"

static void
test_create_and_delete (void **state)
{
  const mdo_allocator_t *alloc = mdo_default_allocator ();

  mdo_ui_ctx_t *ctx;
  mdo_result_t result = mdo_ui_ctx_create (&ctx, alloc);
  assert_true (mdo_result_success (result));

  mdo_ui_ctx_delete (ctx);
}

int
main (void)
{
  const struct CMUnitTest tests[] = {
    cmocka_unit_test (test_create_and_delete),
  };

  return cmocka_run_group_tests (tests, NULL, NULL);
}
