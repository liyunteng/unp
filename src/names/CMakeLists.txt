aux_source_directory(. SRC_LIST)

# add_compile_options(-std=c99 -Wall)
add_executable(hostent hostent.c)
target_link_libraries(hostent unp)

add_executable(daytimetcpclinames daytimetcpcli.c)
target_link_libraries(daytimetcpclinames unp)

# foreach(onefile ${SRC_LIST})
#   message(${onefile})
# endforeach()
