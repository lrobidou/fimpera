include(CTest)
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/tests_bin/)
set (CMAKE_CXX_CPPCHECK "")

file(GLOB_RECURSE SRC_FILE "${PROJECT_SOURCE_DIR}/tests/unit/test_*.cpp")
set(TARGET_NAME ${PROJECT_NAME}-test)
add_executable(${TARGET_NAME} ${SRC_FILE})
add_dependencies(${TARGET_NAME} ${LIB_DEPENDENCIES})
target_include_directories(${TARGET_NAME} PRIVATE ${TEST_INCLUDE_DIR} ${PROJECT_SOURCE_DIR}/tests/unit)
target_link_directories(${TARGET_NAME} PRIVATE ${TEST_LINK_DIR})
target_link_libraries(${TARGET_NAME} ${LIBRARY_NAME} ${TEST_LINK} pthread)

add_test(NAME ${TARGET_NAME} COMMAND sh -c "${PROJECT_BINARY_DIR}/tests_bin/${TARGET_NAME}")

if (CMAKE_BUILD_TYPE STREQUAL "Coverage" AND COVERAGE_BIN STREQUAL "1" AND WITH_COVERAGE)
  target_coverage(coverage-report
                  ${PROJECT_BINARY_DIR}/tests_bin/${TARGET_NAME}
                  ${PROJECT_BINARY_DIR}/coverage_reports)
endif()

if (VALGRIND_FOUND)
  message(STATUS "${BoldGreen}[tests]:${CEnd} add target 'test-memcheck'")
  add_custom_target(test_memcheck
    COMMAND ctest -T memcheck
    WORKING_DIRECTORY "${PROJECT_BINARY_DIR}"
  )
endif()

if (WITH_PROFILE AND CMAKE_BUILD_TYPE STREQUAL "Profile" AND VALGRIND_FOUND)
  set (PROF_OUTPUT ${PROJECT_BINARY_DIR}/profiles)
  file(MAKE_DIRECTORY ${PROJECT_BINARY_DIR}/profiles)
  set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/profile_bin/)
  set (CMAKE_CXX_CPPCHECK "")

  file(GLOB_RECURSE SRC_FILE "${PROJECT_SOURCE_DIR}/tests/profile/profile_*.cpp")
  foreach(PFILE ${SRC_FILE})
    get_filename_component(PNAME ${PFILE} NAME_WE)
    set(TARGET_NAME ${PNAME})
    add_executable(${TARGET_NAME} ${PFILE})
    add_dependencies(${TARGET_NAME} ${LIB_DEPENDENCIES})
    target_include_directories(${TARGET_NAME} PRIVATE ${VALGRIND_INCLUDE_DIR})
    target_link_libraries(${TARGET_NAME} ${LIBRARY_NAME} ${TEST_LINK} pthread)
    add_profile_target("profile-${TARGET_NAME}" "${TARGET_NAME}" "${PROJECT_BINARY_DIR}/profile_bin/${TARGET_NAME}" "${PROF_OUTPUT}")
  endforeach(PFILE SRC_FILE)
endif()

