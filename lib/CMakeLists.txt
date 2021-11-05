file(GLOB_RECURSE LIBRARY_HEADERS "*.hpp")
file(GLOB_RECURSE LIBRARY_SOURCES "*.cpp")
set(LIBRARY_INCLUDE ${PROJECT_SOURCE_DIR}/lib/include ${PROJECT_SOURCE_DIR}/thirdparty/zstr/src ${PROJECT_SOURCE_DIR}/thirdparty/xxhash ${PROJECT_SOURCE_DIR}/thirdparty/cxxopts/include ${PROJECT_SOURCE_DIR}/thirdparty/json/include)# TODO clean

add_library(${LIBRARY_NAME} ${LIBRARY_SOURCES})
add_dependencies(${LIBRARY_NAME} ${LIB_DEPENDENCIES})
target_include_directories(${LIBRARY_NAME} PUBLIC ${LIBRARY_INCLUDE} ${LIB_INCLUDE_DIR})

if (WITH_CLANGFORMAT)
  add_format_lib_target(${LIBRARY_HEADERS} ${LIBRARY_SOURCES})
endif()