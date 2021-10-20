find_path(VALGRIND_INCLUDE_DIR
  valgrind/callgrind.h
  ${VALGRIND_PREFIX}/include
  /usr/include/
  /usr/local/include
)

find_program(VALGRIND_BIN NAMES valgrind PATH
  ${VALGRIND_PREFIX}/bin /usr/bin /usr/local/bin
)

find_package_handle_standard_args(VALGRIND
  DEFAULT_MSG
  VALGRIND_INCLUDE_DIR
  VALGRIND_BIN
)