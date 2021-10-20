#include <gtest/gtest.h>
#include <cpp-app-lib/ex_lib.hpp>

TEST(cpp_app_test_suite_ex_lib, fibonacci)
{
  EXPECT_EQ(cpp_app::fibonacci(10), 55);
}