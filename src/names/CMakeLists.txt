aux_source_directory(. SRC_LIST)

# add_compile_options(-std=c99 -Wall)
foreach (file ${SRC_LIST})
  get_filename_component(exe ${file} NAME_WLE)
  add_executable(${exe} ${file})
  target_link_libraries(${exe} unp)
endforeach(file)

# foreach(onefile ${SRC_LIST})
#   message(${onefile})
# endforeach()
