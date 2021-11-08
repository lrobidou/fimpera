#include <gtest/gtest.h>

#include <fimpera-lib/CBF.hpp>
#include <fimpera-lib/fimpera.hpp>

template class fimpera<countingBF::CBF>;

TEST(fimpera_test_suite_fimpera, fimpera_creation) {
    fimpera<countingBF::CBF> f = fimpera<countingBF::CBF>("../../tests/unit/data/1000LinesTest.txt", 35, 0, false, 10000, 5);  // random  size
}
