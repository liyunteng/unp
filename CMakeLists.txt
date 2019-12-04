cmake_minimum_required(VERSION 3.14)

project(MAIN)
set (PROJECT_VERSION 1.0)

message(STATUS "PROJECT_NAME = " ${PROJECT_NAME})
message(STATUS "PROJECT_VERSION = " ${PROJECT_VERSION})
message(STATUS "CMAKE_BUILD_TYPE = " ${CMAKE_BUILD_TYPE})

message(STATUS "CMAKE_HOST_SYSTEM = " ${CMAKE_HOST_SYSTEM})
message(STATUS "CMAKE_HOST_SYSTEM_NAME = " ${CMAKE_HOST_SYSTEM_NAME})
message(STATUS "CMAKE_HOST_SYSTEM_PROCESSOR = " ${CMAKE_HOST_SYSTEM_PROCESSOR})
message(STATUS "CMAKE_HOST_SYSTEM_VERSION = " ${CMAKE_HOST_SYSTEM_VERSION})

message(STATUS "CMAKE_C_FLAGS = " ${CMAKE_C_FLAGS})
message(STATUS "CMAKE_C_FLAGS_DEBUG = " ${CMAKE_C_FLAGS_DEBUG})
message(STATUS "CMAKE_C_FLAGS_RELEASE = " ${CMAKE_C_FLAGS_RELEASE})

message(STATUS "CMAKE_CXX_FLAGS = " ${CMAKE_CXX_FLAGS})
message(STATUS "CMAKE_CXX_FLAGS_DEBUG = " ${CMAKE_CXX_FLAGS_DEBUG})
message(STATUS "CMAKE_CXX_FLAGS_RELEASE = " ${CMAKE_CXX_FLAGS_RELEASE})

message(STATUS "CMAKE_EXE_LINKER_FLAGS = " ${CMAKE_EXE_LINKER_FLAGS})
message(STATUS "CMAKE_EXE_LINKER_FLAGS_DEBUG = " ${CMAKE_EXE_LINKER_FLAGS_DEBUG})
message(STATUS "CMAKE_EXE_LINKER_FLAGS_RELEASE = " ${CMAKE_EXE_LINKER_FLAGS_RELEASE})

message(STATUS "CMAKE_SHARED_LINKER_FLAGS = " ${CMAKE_SHARED_LINKER_FLAGS})
message(STATUS "CMAKE_SHARED_LINKER_FLAGS_DEBUG = " ${CMAKE_SHARED_LINKER_FLAGS_DEBUG})
message(STATUS "CMAKE_SHARED_LINKER_FLAGS_RELEASE = " ${CMAKE_SHARED_LINKER_FLAGS_RELEASE})

message(STATUS "CMAKE_STATIC_LINKER_FLAGS = " ${CMAKE_STATIC_LINKER_FLAGS})
message(STATUS "CMAKE_STATIC_LINKER_FLAGS_DEBUG = " ${CMAKE_STATIC_LINKER_FLAGS_DEBUG})
message(STATUS "CMAKE_STATIC_LINKER_FLAGS_RELEASE = " ${CMAKE_STATIC_LINKER_FLAGS_RELEASE})

message(STATUS "CMAKE_INCLUDE_PATH = " ${CMAKE_INCLUDE_PATH})
message(STATUS "CMAKE_LIBRARY_PATH = " ${CMAKE_LIBRARY_PATH})
message(STATUS "BUILD_SHARED_LIBS = " ${BUILD_SHARED_LIBS})

######################################################################

option(BUILDSTATIC "build use static library" OFF)
# set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
# set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)
set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)

include_directories(${PROJECT_SOURCE_DIR}/include)
link_directories(${PROJECT_BINARY_DIR}/lib)
# add_definitions(-DNDEBUG)

if (EXISTS ${PROJECT_SOURCE_DIR}/src AND
    IS_DIRECTORY ${PROJECT_SOURCE_DIR}/src)
  add_subdirectory(src)
else()
  message(WARNING "src not exists")
endif()

# find_program(curl "/usr/bin/curl")
# if (curl)
#   exec_program("/usr/bin/curl" ARGS "\"http://www.baidu.com\" -s -o /dev/null" var)
# else()
# endif()

# exec_program(
#   curl
#   ARGS "\"http://www.baidu.com\"" -s
#   OUTPUT_VARIABLE var
#   RETURN_VALUE ret)
# message("var = " ${var})
# message("ret = " ${ret})

# find_package(CURL)
# if (CURL_FOUND)
#   include_directories(${CURL_INCLUDE_DIR})
#   # target_link_directories(test ${CURL_LIBRARY)
#   message("CURL_INCLUDE_DIR = " ${CURL_INCLUDE_DIR})
#   message("CURL_LIBRARY = " ${CURL_LIBRARY})
#   message("CURL library found")
# else(CURL_FOUND)
#   message(FATAL_ERROR "CURL library not found")
# endif(CURL_FOUND)
