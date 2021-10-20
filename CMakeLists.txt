cmake_minimum_required(VERSION 3.13.0)
project(cpp-app VERSION 0.0.1 LANGUAGES C CXX) #TODO

set(PROJECT_DESCRIPTION "cpp-app description") #TODO

set(LIBRARY_NAME "cpp-app-lib") #TODO

option(STATIC_BUILD "Static build." OFF)
option(DEV_BUILD "Dev build." OFF)
option(BUILD_SHARED_LIBS "Build shared library" OFF)

option(WITH_TESTS "Compile tests." ON)
option(WITH_DOC "" ON)
option(WITH_CPPCHECK "" ON)
option(WITH_CLANGFORMAT "" ON)
option(WITH_PROFILE "" ON)
option(WITH_COVERAGE "" ON)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
include(include_all)

if (WITH_CPPCHECK)
  enable_cppcheck()
endif()

if (STATIC_BUILD AND NOT APPLE)
  set(CMAKE_FIND_LIBRARY_SUFFIXES ".a")
  set(CMAKE_EXE_LINKER_FLAGS "-static")
endif()

if (DEV_MODE)
  add_definitions(-D${CPP_APP_DEV_MODE}) #TODO
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS} -O3 -DNDEBUG")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS} -O0 -g")
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS} -O0 -g -DNDEBUG")
set(CMAKE_CXX_FLAGS_PROFILE "${CMAKE_CXX_FLAG} -O2 -g -DNDEBUG")
set(CMAKE_CXX_FLAGS_COVERAGE "${CMAKE_CXX_FLAGS} -O0 -g --coverage")

add_subdirectory(thirdparty)
get_sha1(${PROJECT_SOURCE_DIR} GIT_SHA1)
configure_file_helper("${PROJECT_SOURCE_DIR}/lib/include/config.hpp.in" ${PROJECT_BINARY_DIR}/include/cpp-app-lib) #TODO

add_subdirectory(lib)

add_subdirectory(app)

if (WITH_TESTS)
  enable_testing()
  add_subdirectory(tests)
endif()

if (WITH_DOC)
  add_doxygen_target()
endif()

include(CPackConfig)