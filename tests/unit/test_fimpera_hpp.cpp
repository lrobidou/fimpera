#include <gtest/gtest.h>

#include <fimpera-lib/CBF.hpp>
#include <fimpera-lib/fimpera.hpp>

TEST(fimpera_test_suite_fimpera, fimpera_eq) {
    fimpera<countingBF::CBF> f1 = fimpera<countingBF::CBF>("../../tests/unit/data/1000LinesTest.txt", 35, 0, false, 10000, 5);  // random  size
    fimpera<countingBF::CBF> f2 = fimpera<countingBF::CBF>("../../tests/unit/data/1000LinesTest.txt", 35, 0, false, 10000, 1);  // random size
    EXPECT_FALSE(f1 == f2);
}

TEST(fimpera_test_suite_fimpera, fimpera_not_eq) {
    fimpera<countingBF::CBF> f1 = fimpera<countingBF::CBF>("../../tests/unit/data/1000LinesTest.txt", 35, 0, false, 10000, 5);  // random  size
    fimpera<countingBF::CBF> f2 = fimpera<countingBF::CBF>("../../tests/unit/data/1000LinesTest.txt", 35, 1, false, 10000, 5);  // random size
    EXPECT_FALSE(f1 == f2);
}

TEST(fimpera_test_suite_fimpera, readUUID) {
    std::string filename = "../../tests/unit/data/test.idx";
    std::ifstream fin(filename, std::ios::out | std::ofstream::binary);
    EXPECT_EQ(readUUID(fin), "23e0132d-0e3d-4639-9794-69c2e22a0af4");
}