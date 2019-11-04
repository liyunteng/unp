include_directories(${PROJECT_SOURCE_DIR}/include)
aux_source_directory(. SRC_LIST)

# add_compile_options(-std=c99 -Wall)

link_directories(${PROJECT_SOURCE_DIR}/lib)
add_executable(hello ${SRC_LIST})
target_link_libraries(hello unp)

# foreach(onefile ${SRC_LIST})
#   message(${onefile})
# endforeach()
