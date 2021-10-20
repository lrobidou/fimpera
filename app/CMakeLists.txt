file(GLOB_RECURSE APP_SOURCES "*.cpp")

set(APP_INCLUDE ${PROJECT_SOURCE_DIR}/app/src)
add_executable(${PROJECT_NAME} ${APP_SOURCES})
add_dependencies(${PROJECT_NAME} ${APP_DEPENDENCIES})
target_include_directories(${PROJECT_NAME} PRIVATE ${APP_INCLUDE} ${APP_INCLUDE_DIR})
target_link_libraries(${PROJECT_NAME} ${LIBRARY_NAME} ${APP_LINK})

if (WITH_CLANGFORMAT)
  add_format_app_target(${APP_SOURCES})
endif()