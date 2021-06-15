/** @file test_ui_draw_list.c
 */

#include "test_common.h"
#include "ui_draw_list.h"

static void
test_create_and_delete (void **state)
{
  const mdo_allocator_t *alloc = mdo_default_allocator ();

  mdo_ui_draw_list_t *ui_draw;
  mdo_result_t result = mdo_ui_draw_list_create (&ui_draw, alloc);
  assert_true (mdo_result_success (result));

  mdo_ui_draw_list_delete (ui_draw);
}

int
main ()
{
  const struct CMUnitTest tests[] = {
    cmocka_unit_test (test_create_and_delete),
  };

  return cmocka_run_group_tests (tests, NULL, NULL);
}
