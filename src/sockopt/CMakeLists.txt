aux_source_directory(. SRC_LIST)

# add_compile_options(-std=c99 -Wall)
add_executable(checkopt checkopt.c)
target_link_libraries(checkopt unp)

# foreach(onefile ${SRC_LIST})
#   message(${onefile})
# endforeach()
