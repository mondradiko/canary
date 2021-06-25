include_directories (.)

include (mondradiko_create_test)
mondradiko_create_test (${CANARY_OBJ} test_draw_list unit/test_draw_list.c)

option (ENABLE_GLFW_HARNESS "Enable the GLFW test harness.")

if (ENABLE_GLFW_HARNESS)
  set (BUILD_GLES_RENDERER 1)
endif ()

if (BUILD_GLES_RENDERER)
  add_library (gles-renderer STATIC harness/gles-renderer/gles_renderer.c)

  # TODO(marceline-cramer): this is hardcoded for my system; find loader lib
  target_link_libraries (gles-renderer GLESv2)
endif ()

if (ENABLE_GLFW_HARNESS)
  find_package (glfw3 3.3 REQUIRED)

  add_executable (test-harness-glfw harness/glfw/main.c)

  target_link_libraries (test-harness-glfw
    ${CANARY_OBJ}
    gles-renderer
    glfw
  )
endif ()

