include_directories (.)

include (mondradiko_create_test)
mondradiko_create_test (${MDO_UI_OBJ} test_ui_ctx unit/test_ui_ctx.c)

add_subdirectory (vk-bootstrap)
find_package (SDL2 CONFIG REQUIRED)

add_executable (test-harness-sdl harness/sdl/main.c harness/sdl/vk_renderer.c)
target_link_libraries (test-harness-sdl ${MDO_UI_OBJ} vk-bootstrap SDL2::SDL2)
