set (WASMTIME_VERSION "v0.28.0")
set (WASMTIME_ARCH "x86_64")

if (UNIX)
  set (WASMTIME_OS "linux")
  set (WASMTIME_EXTENSION "tar.xz")
else ()
  message (WARNING "Wasmtime on Windows is untested; please make an issue about your results if you try it")
  set (WASMTIME_OS "windows")
  set (WASMTIME_EXTENSION "zip")
endif ()

set (WASMTIME_SUBDIR "wasmtime-${WASMTIME_VERSION}-${WASMTIME_ARCH}-${WASMTIME_OS}-c-api")
set (WASMTIME_NAME "${WASMTIME_SUBDIR}.${WASMTIME_EXTENSION}")
set (WASMTIME_URL "https://github.com/bytecodealliance/wasmtime/releases/download/${WASMTIME_VERSION}/${WASMTIME_NAME}")
set (WASMTIME_ARCHIVE "${CMAKE_CURRENT_BINARY_DIR}/${WASMTIME_NAME}")

# delete downloaded archive to redo download and decompression
if (NOT EXISTS ${WASMTIME_ARCHIVE})
  message (STATUS "Downloading Wasmtime (${WASMTIME_NAME})")

  file (
    DOWNLOAD ${WASMTIME_URL} ${WASMTIME_ARCHIVE}
    TIMEOUT 60 # one minute
  )

  file (ARCHIVE_EXTRACT INPUT ${WASMTIME_ARCHIVE})
endif ()

add_library (wasmtime::wasmtime STATIC IMPORTED GLOBAL)
set_property (TARGET wasmtime::wasmtime PROPERTY IMPORTED_LOCATION
  "${CMAKE_CURRENT_BINARY_DIR}/${WASMTIME_SUBDIR}/lib/libwasmtime.a")
set_property (TARGET wasmtime::wasmtime PROPERTY INTERFACE_INCLUDE_DIRECTORIES
  "${CMAKE_CURRENT_BINARY_DIR}/${WASMTIME_SUBDIR}/include/")

