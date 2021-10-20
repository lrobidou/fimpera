find_program(CLANG_FORMAT_BIN NAMES "clang-format" DOC "Path to clang-format executable")

macro(add_format_lib_target HEADER_FILES SOURCE_FILES)
  if(CLANG_FORMAT_BIN)
    message(STATUS "${BoldGreen}[clang-format]${CEnd}: add target 'format-lib'")
    add_custom_target(
      format-lib
      COMMAND ${CLANG_FORMAT_BIN} -i -style=file ${HEADER_FILES} ${SOURCE_FILES}
      COMMAND_EXPAND_LISTS VERBATIM
    )
  endif()
endmacro()

macro(add_format_app_target SOURCE_FILES)
  if(CLANG_FORMAT_BIN)
    message(STATUS "${BoldGreen}[clang-format]${CEnd}: add target 'format-app'")
    add_custom_target(
      format-app
      COMMAND ${CLANG_FORMAT_BIN} -i -style=file ${SOURCE_FILES}
      COMMAND_EXPAND_LISTS VERBATIM
    )
  endif()
endmacro()
