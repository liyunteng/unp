include_directories(${PROJECT_SOURCE_DIR}/include)
aux_source_directory(. SRC_LIST)

add_library(lib_shared SHARED ${SRC_LIST})
set_target_properties(lib_shared PROPERTIES OUTPUT_NAME "unp")

if (BUILDSTATIC)
  add_library(lib_static STATIC ${SRC_LIST})
  set_target_properties(lib_static PROPERTIES OUTPUT_NAME "unp")
else ()
  message("Don't build static library")
endif()

# set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)
set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)
