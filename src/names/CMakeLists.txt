aux_source_directory(. SRC_LIST)

# add_compile_options(-std=c99 -Wall)
add_executable(hostent hostent.c)
target_link_libraries(hostent unp)

add_executable(getaddrinfo getaddrinfo.c)
target_link_libraries(getaddrinfo unp)

add_executable(tcpclient tcpclient.c)
target_link_libraries(tcpclient unp)

add_executable(tcpserver tcpserver.c)
target_link_libraries(tcpserver unp)

add_executable(daytimetcpclinames01 daytimetcpclinames01.c)
target_link_libraries(daytimetcpclinames01 unp)

add_executable(daytimetcpsrvnames01 daytimetcpsrv01.c)
target_link_libraries(daytimetcpsrvnames01 unp)

add_executable(tcp_connect tcp_connect.c)
target_link_libraries(tcp_connect unp)

# foreach(onefile ${SRC_LIST})
#   message(${onefile})
# endforeach()
