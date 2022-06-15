#include <gtest/gtest.h>

#include <fimpera-lib/CBF.hpp>
#include <fimpera-lib/fimpera.hpp>

template class fimpera<countingBF::CBF>;

class ResultGetter : public CustomResponse {
   private:
    std::vector<int> entireResponse;

   public:
    ResultGetter() {}

    void
    processResult(const std::vector<int>& res, const unsigned int& K, const std::string& current_header, const std::string& current_read) {
        entireResponse.insert(std::end(entireResponse), std::begin(res), std::end(res));
    }

    std::vector<int> getResult() {
        return entireResponse;
    }
};

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

TEST(fimpera_test_suite_fimpera, fimpera_get_cannical) {
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

inline void printv(const std::vector<int>& x) {
    for (auto y : x) {
        std::cout << y << " ";
    }
    std::cout << std::endl;
}

TEST(fimpera_test_suite_fimpera, queryRead_z3) {
    const std::string filename = "../../tests/unit/data/1000LinesTest.txt";
    const std::string common0 = "AAACAGCGACGGAAGATAGCGTTGCTCCAGCAGTTG";
    const std::string common1 = "AAACAGCGACTCTGAATATAGCGAATTTGATCACA";

    const std::string read = common0 + common1;

    fimpera<countingBF::CBF> f = fimpera<countingBF::CBF>(filename, 35, 3, false, 100000000, 5);  // size high enough to prevent false positives

    std::vector<int> expected = {1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1};
    EXPECT_EQ(f.queryRead(read), expected);
}

TEST(fimpera_test_suite_fimpera, queryRead_z0) {
    const std::string filename = "../../tests/unit/data/1000LinesTest.txt";
    const std::string common0 = "AAACAGCGACGGAAGATAGCGTTGCTCCAGCAGTTG";
    const std::string common1 = "AAACAGCGACTCTGAATATAGCGAATTTGATCACA";

    const std::string read = common0 + common1;

    fimpera<countingBF::CBF> f = fimpera<countingBF::CBF>(filename, 35, 0, false, 100000000, 5);  // size high enough to prevent false positives

    std::vector<int> expected = {1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1};
    EXPECT_EQ(f.queryRead(read), expected);
}

TEST(fimpera_test_suite_fimpera, queryRead_canonical_not_required) {
    // use canonical parameter on not canonical input => should do nothing
    const std::string filename = "../../tests/unit/data/1000LinesTest.txt";
    const std::string common0 = "AAACAGCGACGGAAGATAGCGTTGCTCCAGCAGTTG";
    const std::string common1 = "AAACAGCGACTCTGAATATAGCGAATTTGATCACA";

    const std::string read = common0 + common1;

    fimpera<countingBF::CBF> f = fimpera<countingBF::CBF>(filename, 35, 3, true, 100000000, 5);  // size high enough to prevent false positives

    std::vector<int> expected = {1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1};
    EXPECT_EQ(f.queryRead(read), expected);
}

TEST(fimpera_test_suite_fimpera, queryRead_canonical) {
    const std::string filename = "../../tests/unit/data/1000LinesTest.txt";
    const std::string common0 = "CAACTGCTGGAGCAACGCTATCTTCCGTCGCTGTTT";
    const std::string common1 = "AAACAGCGACTCTGAATATAGCGAATTTGATCACA";

    const std::string read = common0 + common1;

    fimpera<countingBF::CBF> f = fimpera<countingBF::CBF>(filename, 35, 3, true, 100000000, 5);  // size high enough to prevent false positives

    std::vector<int> expected = {1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1};
    EXPECT_EQ(f.queryRead(read), expected);
}

TEST(fimpera_test_suite_fimpera, queryRead_wrongly_canonical) {
    // we query the canonical but we ask no canonical
    const std::string filename = "../../tests/unit/data/1000LinesTest.txt";
    const std::string common0 = "CAACTGCTGGAGCAACGCTATCTTCCGTCGCTGTTT";
    const std::string common1 = "AAACAGCGACTCTGAATATAGCGAATTTGATCACA";

    const std::string read = common0 + common1;

    fimpera<countingBF::CBF> f = fimpera<countingBF::CBF>(filename, 35, 3, false, 100000000, 5);  // size high enough to prevent false positives

    std::vector<int> expected = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1};
    EXPECT_EQ(f.queryRead(read), expected);
}

TEST(fimpera_test_suite_fimpera, query) {
    const std::string fasta_filename = "../../tests/unit/data/rnaseq_shortened.fasta";
    const std::string kmc_filename = "../../tests/unit/data/rnaseq_shortened.txt";

    ResultGetter result_getter = ResultGetter();

    fimpera<countingBF::CBF> f = fimpera<countingBF::CBF>(kmc_filename, 35, 3, false, 100000000, 5);  // size high enough to prevent false positives
    f.query(fasta_filename, result_getter);
    std::vector<int> response = result_getter.getResult();

    std::vector<int> expected = {1, 1, 1, 1, 0, -1, 1, 1, 1, 1};
    EXPECT_EQ(response, expected);
}

TEST(fimpera_test_suite_fimpera, testStrategyEqualityDefault) {
    const std::string f1 = "../../tests/unit/data/index_1000LinesTest_identity_strategy_as_default.idx";
    const std::string f2 = "../../tests/unit/data/test1000Lines.idx";

    fimpera<countingBF::CBF> identityIdx = fimpera<countingBF::CBF>(f1);
    fimpera<countingBF::CBF> noStrategyIdx = fimpera<countingBF::CBF>(f2);

    EXPECT_EQ(identityIdx, noStrategyIdx);
}

TEST(fimpera_test_suite_fimpera, testStrategyEqualityChosen) {
    const std::string f1 = "../../tests/unit/data/index_1000LinesTest_identity_strategy_chosen.idx";
    const std::string f2 = "../../tests/unit/data/test1000Lines.idx";

    fimpera<countingBF::CBF> identityIdx = fimpera<countingBF::CBF>(f1);
    fimpera<countingBF::CBF> noStrategyIdx = fimpera<countingBF::CBF>(f2);

    EXPECT_EQ(identityIdx, noStrategyIdx);
}

TEST(fimpera_test_suite_fimpera, testStrategyInequality) {
    const std::string f1 = "../../tests/unit/data/index_1000LinesTest_log2_strategy.idx";
    const std::string f2 = "../../tests/unit/data/test1000Lines.idx";

    fimpera<countingBF::CBF> log2 = fimpera<countingBF::CBF>(f1);
    fimpera<countingBF::CBF> noStrategyIdx = fimpera<countingBF::CBF>(f2);

    EXPECT_FALSE(log2 == noStrategyIdx);
}

TEST(fimpera_test_suite_fimpera, testStrategyInequalitySameContentDifferentName) {
    const std::string f1 = "../../tests/unit/data/index_1000LinesTest_identity_strategy_but_the_inner_name_is_called_log2.idx";
    const std::string f2 = "../../tests/unit/data/test1000Lines.idx";

    fimpera<countingBF::CBF> log2InName = fimpera<countingBF::CBF>(f1);
    fimpera<countingBF::CBF> defaultIdx = fimpera<countingBF::CBF>(f2);

    EXPECT_FALSE(log2InName == defaultIdx);
}

TEST(fimpera_test_suite_fimpera, testStrategyInequalitySameNameDifferentContent) {
    const std::string f1 = "../../tests/unit/data/index_1000LinesTest_identity_strategy_but_the_inner_name_is_called_log2.idx";
    const std::string f2 = "../../tests/unit/data/index_1000LinesTest_log2_strategy.idx";

    fimpera<countingBF::CBF> identityIdx = fimpera<countingBF::CBF>(f1);
    fimpera<countingBF::CBF> defaultIdx = fimpera<countingBF::CBF>(f2);

    EXPECT_FALSE(identityIdx == defaultIdx);
}