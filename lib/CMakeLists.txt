add_library (wasmtime::wasmtime STATIC IMPORTED GLOBAL)
set_property (TARGET wasmtime::wasmtime PROPERTY
	IMPORTED_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}/libwasmtime.a")

