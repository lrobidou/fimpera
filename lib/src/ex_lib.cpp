#include <cpp-app-lib/ex_lib.hpp>

namespace cpp_app
{

int fibonacci(int n)
{
  if (n < 2)
  {
    return n;
  }
  else
  {
    return fibonacci(n - 1) + fibonacci(n - 2);
  }
}

} // namespace cpp_app
