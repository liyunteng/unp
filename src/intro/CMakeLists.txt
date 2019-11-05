include_directories(${PROJECT_SOURCE_DIR}/include)
aux_source_directory(. SRC_LIST)

# add_compile_options(-std=c99 -Wall)

link_directories(${PROJECT_SOURCE_DIR}/lib)
add_executable(daytimetcpcli daytimetcpcli.c)
target_link_libraries(daytimetcpcli unp)

add_executable(daytimetcpsrv daytimetcpsrv.c)
target_link_libraries(daytimetcpsrv unp)

add_executable(daytimetcpsrv1 daytimetcpsrv1.c)
target_link_libraries(daytimetcpsrv1 unp)

add_executable(byteorder byteorder.c)

# foreach(onefile ${SRC_LIST})
#   message(${onefile})
# endforeach()
