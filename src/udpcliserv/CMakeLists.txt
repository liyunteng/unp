aux_source_directory(. SRC_LIST)

# add_compile_options(-std=c99 -Wall)
add_executable(udpserv01 udpserv01.c)
target_link_libraries(udpserv01 unp)

add_executable(udpcli01 udpcli01.c)
target_link_libraries(udpcli01 unp)

# foreach(onefile ${SRC_LIST})
#   message(${onefile})
# endforeach()
