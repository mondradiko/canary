include_directories (.)

include (mondradiko_create_test)
mondradiko_create_test (${CANARY_OBJ} test_draw_list unit/test_draw_list.c)

add_subdirectory (vk-bootstrap)
find_package (SDL2 CONFIG REQUIRED)
include (FindVulkan REQUIRED)

# fix SDL2 import on Arch and vcpkg
if (TARGET SDL2::SDL2)
	set (SDL2_LIBRARIES SDL2::SDL2)
endif ()

add_executable (test-harness-sdl harness/sdl/main.c harness/sdl/vk_renderer.cpp)

target_link_libraries (test-harness-sdl
	${CANARY_OBJ}
	vk-bootstrap
	${SDL2_LIBRARIES}
	${Vulkan_LIBRARIES}
)


target_include_directories (test-harness-sdl PUBLIC
	${SDL2_INCLUDE_DIRS}
	${Vulkan_INCLUDE_DIRS}
)

