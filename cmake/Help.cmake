add_custom_target(app-help
        ${CMAKE_COMMAND} -E cmake_echo_color --cyan
        "Binary: ${PROJECT_BINARY_DIR}/app"
        "Library: ${PROJECT_BINARY_DIR}/lib"
        "Configuratin header: ${PROJECT_BINARY_DIR}/include"
        "Tests: ${PROJECT_BINARY_DIR}/tests_bin"
        "Coverage: ${PROJECT_BINARY_DIR}/coverage-report"
        "CppCheck: ${PROJECT_BINARY_DIR}/cppcheck-report"
        "Profiling: ${PROJECT_BINARY_DIR}/profiles"
        "Documentation: ${PROJECT_BINARY_DIR}/documentation"
        COMMENT "" )