include_directories (.)

include (mondradiko_create_test)
mondradiko_create_test (${CANARY_OBJ} test_draw_list unit/test_draw_list.c)

option (ENABLE_SDL_HARNESS "Enable the SDL2 test harness.")

if (ENABLE_SDL_HARNESS)
  add_subdirectory (vk-bootstrap)
  find_package (SDL2 CONFIG REQUIRED)
  include (FindVulkan REQUIRED)

  add_executable (test-harness-sdl harness/sdl/main.c harness/sdl/vk_renderer.cpp)

  target_link_libraries (test-harness-sdl
    ${CANARY_OBJ}
    vk-bootstrap
		SDL2::SDL2
    ${Vulkan_LIBRARIES}
  )
endif ()

