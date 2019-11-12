aux_source_directory(. SRC_LIST)

# add_compile_options(-std=c99 -Wall)
add_executable(tcpserv01 tcpserv01.c)
target_link_libraries(tcpserv01 unp)

add_executable(tcpcli01 tcpcli01.c)
target_link_libraries(tcpcli01 unp)

add_executable(tcpserv04 tcpserv04.c)
target_link_libraries(tcpserv04 unp)

add_executable(tcpcli04 tcpcli04.c)
target_link_libraries(tcpcli04 unp)

add_executable(tcpserv08 tcpserv08.c)
target_link_libraries(tcpserv08 unp)

add_executable(tcpcli08 tcpcli08.c)
target_link_libraries(tcpcli08 unp)

add_executable(tcpserv09 tcpserv09.c)
target_link_libraries(tcpserv09 unp)

add_executable(tcpcli09 tcpcli09.c)
target_link_libraries(tcpcli09 unp)

add_executable(tcpservselect01 tcpservselect01.c)
target_link_libraries(tcpservselect01 unp)

add_executable(tcpcliselect01 tcpcliselect01.c)
target_link_libraries(tcpcliselect01 unp)

# foreach(onefile ${SRC_LIST})
#   message(${onefile})
# endforeach()
