# file(GLOB_RECURSE APP_SOURCES "*.cpp")

set(APP_SOURCE_DIR ${PROJECT_SOURCE_DIR}/app/src)

# set(APP_INCLUDE ${PROJECT_SOURCE_DIR}/app/src)
add_executable(fimpera_index ${APP_SOURCE_DIR}/index.cpp)
add_dependencies(fimpera_index ${APP_DEPENDENCIES})
# target_include_directories(fimpera_index PRIVATE ${APP_INCLUDE} ${APP_INCLUDE_DIR})
target_link_libraries(fimpera_index ${LIBRARY_NAME} ${APP_LINK} z)

# TODO ask what this is about
# if (WITH_CLANGFORMAT)
#   add_format_app_target(${APP_SOURCES})
# endif()

add_executable(fimpera_info ${APP_SOURCE_DIR}/infos.cpp)
add_dependencies(fimpera_info ${APP_DEPENDENCIES})
target_link_libraries(fimpera_info ${LIBRARY_NAME} ${APP_LINK} z)

add_executable(fimpera_query ${APP_SOURCE_DIR}/query.cpp)
add_dependencies(fimpera_query ${APP_DEPENDENCIES})
target_link_libraries(fimpera_query ${LIBRARY_NAME} ${APP_LINK} z)

add_executable(fimpera_truth ${APP_SOURCE_DIR}/truth.cpp)
add_dependencies(fimpera_truth ${APP_DEPENDENCIES})
target_link_libraries(fimpera_truth ${LIBRARY_NAME} ${APP_LINK} z)