#include <gtest/gtest.h>

#include <fimpera-lib/files.hpp>

TEST(fimpera_test_suite_files, fileExists) {
    std::string filename = "../../tests/unit/data/1000LinesTest.txt";
    EXPECT_EQ(fileExists(filename), true);
}

TEST(fimpera_test_suite_files, fileDoesNotExist) {
    std::string filename = "../../tests/unit/data/1000LinesTestblabla.txt";
    EXPECT_EQ(fileExists(filename), false);
}