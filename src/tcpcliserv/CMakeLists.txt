include_directories(${PROJECT_SOURCE_DIR}/include)
aux_source_directory(. SRC_LIST)

# add_compile_options(-std=c99 -Wall)

link_directories(${PROJECT_SOURCE_DIR}/lib)
add_executable(tcpserv01 tcpserv01.c)
target_link_libraries(tcpserv01 unp)

add_executable(tcpcli01 tcpcli01.c)
target_link_libraries(tcpcli01 unp)

# foreach(onefile ${SRC_LIST})
#   message(${onefile})
# endforeach()
