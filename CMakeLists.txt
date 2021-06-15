# Copyright (c) 2021 Marceline Cramer
# SPDX-License-Identifier: MIT

cmake_minimum_required (VERSION 3.18)
project (mdo-ui VERSION 0.0.0)

# options
option (ENABLE_TESTS "Enable testing suite.")

# C standard
set (CMAKE_C_STANDARD 99)
set (CMAKE_C_STANDARD_REQUIRED TRUE)
add_compile_options (-Wpedantic)

# dependencies
find_package (mdo-utils REQUIRED)
find_package (Threads REQUIRED)

# wasmtime
set (WASMTIME_LIBRARIES "${CMAKE_CURRENT_SOURCE_DIR}/lib/libwasmtime.a")

# setup library
include (mondradiko_setup_library)
mondradiko_setup_library (mdo-ui MDO_UI_OBJ
  src/ui_ctx.c
  src/ui_panel.c
  src/ui_script.c
)

target_link_libraries (${MDO_UI_OBJ}
  mondradiko::mdo-utils
  ${WASMTIME_LIBRARIES}
  Threads::Threads
  ${CMAKE_DL_LIBS}
  m
)

target_include_directories (${MDO_UI_OBJ} PUBLIC lib)

# tests
if (ENABLE_TESTS)
  enable_testing ()
  add_subdirectory (tests)
endif ()
