include_directories(${PROJECT_SOURCE_DIR}/include)
aux_source_directory(. SRC_LIST)

add_library(libunp_shared SHARED ${SRC_LIST})
set_target_properties(libunp_shared PROPERTIES OUTPUT_NAME "unp")

if (BUILDSTATIC)
  add_library(libunp_static STATIC ${SRC_LIST})
  set_target_properties(libunp_static PROPERTIES OUTPUT_NAME "unp")
else ()
  message(STATUS "Don't build static library")
endif()
