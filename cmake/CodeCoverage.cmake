
find_program(GCOV_BIN gcov)
find_program(LCOV_BIN lcov)
find_program(GENHTML_BIN genhtml)

if (NOT GCOV_BIN AND NOT LCOV_BIN AND NOT GENHTML_BIN)
  set(COVERAGE_BIN 0)
else()
  set(COVERAGE_BIN 1)
endif()

function(target_coverage target_name runners report_directory)
  message(STATUS "${BoldGreen}[coverage]${CEnd}: add target '${target_coverage}'")
  add_custom_target(${target_name}
    COMMAND ${runners}
    COMMAND ${LCOV_BIN} --directory . --capture --output-file ${report_directory}.info
    COMMAND ${LCOV_BIN} --remove ${report_directory}.info '*/tests/*' '*/thirdparty/*' '*/app/*' '/usr*' --output-file ${report_directory}.info.filtered
    COMMAND ${GENHTML_BIN} -o ${report_directory} ${report_directory}.info.filtered
    COMMAND ${LCOV_BIN} --list ${report_directory}.info.filtered
    WORKING_DIRECTORY ${PROJECT_BINARY_DIR})
  add_custom_command(TARGET ${target_name} POST_BUILD COMMENT "Coverage reports generated at ${report_directory}")
endfunction()