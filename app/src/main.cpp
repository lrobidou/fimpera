#include <cpp-app-lib/ex_lib.hpp>
#include <iostream>

int main(int argc, char* argv[])
{
  std::cout << "Fibonacci(10) = " << std::to_string(cpp_app::fibonacci(10)) << std::endl;
  return 0;
}