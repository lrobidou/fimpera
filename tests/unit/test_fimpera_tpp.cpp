#include <gtest/gtest.h>

#include <fimpera-lib/CBF.hpp>
#include <fimpera-lib/fimpera.hpp>

template class fimpera<countingBF::CBF>;

TEST(fimpera_test_suite_fimpera, fimpera_creation) {
    fimpera<countingBF::CBF> f = fimpera<countingBF::CBF>("../../tests/unit/data/1000LinesTest.txt", 35, 0, false, 10000, 5);  // random  size
}

TEST(fimpera_test_suite_fimpera, fimpera_creation_no_file) {
    const std::string filename = "../../tests/unit/data/1000LinesTestblabla.txt";
    const std::string msg = "The file " + filename + " does not exist.";
    try {
        fimpera<countingBF::CBF> f = fimpera<countingBF::CBF>(filename, 35, 0, false, 10000, 5);  // random  size
        FAIL() << "Expected std::runtime_error";
    } catch (const std::runtime_error& e) {
        EXPECT_EQ(e.what(), msg);
    } catch (...) {
        FAIL() << "Expected std::runtime_error";
    }
}

TEST(fimpera_test_suite_files, fimpera_creation_second_constructor) {
    fimpera<countingBF::CBF> f1 = fimpera<countingBF::CBF>("../../tests/unit/data/1000LinesTest.txt", 35, 3, false, 10000, 5);
    std::string filename = "../../tests/unit/data/test1000Lines.idx";
    fimpera<countingBF::CBF> f2 = fimpera<countingBF::CBF>(filename);

    EXPECT_TRUE(f1 == f2);
}

TEST(fimpera_test_suite_files, fimpera_creation_second_constructor_wrong_file) {
    const std::string filename = "../../tests/unit/data/1000LinesTest.txt";
    std::string indexFilename = "../../tests/unit/data/test.idx";  // this index was not computed from the above file

    fimpera<countingBF::CBF> f1 = fimpera<countingBF::CBF>(filename, 35, 3, false, 10000, 5);
    fimpera<countingBF::CBF> f2 = fimpera<countingBF::CBF>(indexFilename);

    EXPECT_FALSE(f1 == f2);
}

TEST(fimpera_test_suite_files, fimpera_save) {
    std::string filename = "../../tests/unit/data/test1000Lines.idx";
    const std::string filenameToSaveTo = "../../tests/unit/data/test1000Lines_from_unit_test.idx";

    fimpera<countingBF::CBF> f1 = fimpera<countingBF::CBF>("../../tests/unit/data/1000LinesTest.txt", 35, 3, false, 10000, 5);
    f1.save(filenameToSaveTo);
    fimpera<countingBF::CBF> f2 = fimpera<countingBF::CBF>(filename);
    fimpera<countingBF::CBF> f3 = fimpera<countingBF::CBF>(filenameToSaveTo);

    EXPECT_TRUE(f2 == f3);
}

TEST(fimpera_test_suite_fimpera, fimpera_get_canonical) {
    const std::string filename = "../../tests/unit/data/1000LinesTest.txt";
    fimpera<countingBF::CBF> f = fimpera<countingBF::CBF>(filename, 35, 0, false, 10000, 5);  // random  size
    EXPECT_EQ(f.getCanonical(), false);
}

TEST(fimpera_test_suite_fimpera, fimpera_get_K) {
    const std::string filename = "../../tests/unit/data/1000LinesTest.txt";
    fimpera<countingBF::CBF> f = fimpera<countingBF::CBF>(filename, 35, 0, false, 10000, 5);  // random  size
    EXPECT_EQ(f.getK(), 35);
}

TEST(fimpera_test_suite_fimpera, fimpera_get_z_0) {
    const std::string filename = "../../tests/unit/data/1000LinesTest.txt";
    fimpera<countingBF::CBF> f = fimpera<countingBF::CBF>(filename, 35, 0, false, 10000, 5);  // random  size
    EXPECT_EQ(f.getz(), 0);
}

TEST(fimpera_test_suite_fimpera, fimpera_get_z_3) {
    const std::string filename = "../../tests/unit/data/1000LinesTest.txt";
    fimpera<countingBF::CBF> f = fimpera<countingBF::CBF>(filename, 35, 3, false, 10000, 5);  // random  size
    EXPECT_EQ(f.getz(), 3);
}