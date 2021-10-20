#include <valgrind/callgrind.h>

#include <cpp-app-lib/ex_lib.hpp>

int main(void)
{
  CALLGRIND_START_INSTRUMENTATION;
  CALLGRIND_TOGGLE_COLLECT;
  cpp_app::fibonacci(30);
  CALLGRIND_TOGGLE_COLLECT;
  CALLGRIND_STOP_INSTRUMENTATION;
  return 0;
}