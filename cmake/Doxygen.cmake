find_package(Doxygen)

macro(add_doxygen_target)
  if (DOXYGEN_FOUND)
    message(STATUS "${BoldGreen}[doxygen]${CEnd}: add target 'documentation'")
    file(MAKE_DIRECTORY ${PROJECT_BINARY_DIR}/documentation)

    add_custom_target(documentation
                      ${DOXYGEN_EXECUTABLE} ${CMAKE_SOURCE_DIR}/Doxyfile &> doxygen.log
                      WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/documentation)
  endif()
endmacro()