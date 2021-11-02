#include <gtest/gtest.h>

#include <fimpera-lib/CBF.hpp>
#include <fimpera-lib/finderec.hpp>

TEST(fimpera_test_suite_finderec, range_error) {
    int K = 31;
    int z = 0;
    std::string query = "ACT";
    std::vector<int> expected = {};
    countingBF::CBF cbf = countingBF::CBF(1000, 5);
    std::string expected_error = "`query.length()` (which is " + std::to_string(query.length()) + ") < `K` (which is " + std::to_string(K) + ")";

    try {
        std::vector<int> res = finderec(cbf, query, K, z, false);
        FAIL() << "Expected std::range_error";
    } catch (std::range_error const& err) {
        EXPECT_EQ(err.what(), expected_error);
    } catch (...) {
        FAIL() << "Expected std::range_error";
    }
}

TEST(fimpera_test_suite_finderec, negative_stretch_size1) {
    int K = 31;
    int z = 0;
    std::string query = "ACTTTTTTTTTTTCGGTGCTGCTTAGGATAA";
    std::vector<int> expected = {0};
    countingBF::CBF cbf = countingBF::CBF(1000, 5);

    std::vector<int> res = finderec(cbf, query, K, z, false);

    EXPECT_EQ(res, expected);
}

TEST(fimpera_test_suite_finderec, negative_stretch_size9) {
    int K = 31;
    int z = 0;
    std::string query = "ACTTTTTTTTTTTCGGTGCTGCTTAGGATAAGAAAAAAA";
    std::vector<int> expected = {0, 0, 0, 0, 0, 0, 0, 0, 0};
    countingBF::CBF cbf = countingBF::CBF(1000, 5);

    std::vector<int> res = finderec(cbf, query, K, z, false);

    EXPECT_EQ(res, expected);
}

TEST(fimpera_test_suite_finderec, negative_and_positive_stretch_1) {
    // no influence of a single positive kmer
    int K = 12;
    int z = 3;
    std::string query = "0123456789ABCDEFGHIJKL";
    std::vector<int> expected = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    countingBF::CBF cbf = countingBF::CBF(1000, 5);
    cbf.set("123456789");

    std::vector<int> res = finderec(cbf, query, K, z, false);

    EXPECT_EQ(res, expected);
}

TEST(fimpera_test_suite_finderec, negative_and_positive_stretch_2) {
    // be able to find a stretch
    int K = 12;
    int z = 3;
    std::string query = "0123456789ABCDEFGHIJKL";
    std::vector<int> expected = {0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    countingBF::CBF cbf = countingBF::CBF(1000, 5);
    cbf.set("123456789");
    cbf.set("23456789A");
    cbf.set("3456789AB");
    cbf.set("456789ABC");

    std::vector<int> res = finderec(cbf, query, K, z, false);

    EXPECT_EQ(res, expected);
}

TEST(fimpera_test_suite_finderec, negative_and_positive_stretch_3) {
    // be able to find a stretch and its abundance
    int K = 12;
    int z = 3;
    std::string query = "0123456789ABCDEFGHIJKL";
    std::vector<int> expected = {0, 2, 1, 1, 1, 0, 0, 0, 0, 0, 0};
    countingBF::CBF cbf = countingBF::CBF(1000, 5);
    cbf.set("123456789", 2);
    cbf.set("23456789A", 2);
    cbf.set("3456789AB", 2);
    cbf.set("456789ABC", 2);
    cbf.set("56789ABCD");
    cbf.set("6789ABCDE");
    cbf.set("789ABCDEF");
    cbf.set("9ABCDEFGH");

    std::vector<int> res = finderec(cbf, query, K, z, false);

    EXPECT_EQ(res, expected);
}

TEST(fimpera_test_suite_finderec, negative_and_positive_stretch_4) {
    // be able to find a stretch and its abundance
    int K = 12;
    int z = 3;
    std::string query = "0123456789ABCDEFGHIJKL";
    std::vector<int> expected = {0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1};
    countingBF::CBF cbf = countingBF::CBF(1000, 5);
    cbf.set("9ABCDEFGH");
    cbf.set("ABCDEFGHI");
    cbf.set("BCDEFGHIJ");
    cbf.set("CDEFGHIJK");
    cbf.set("DEFGHIJKL");

    std::vector<int> res = finderec(cbf, query, K, z, false);

    EXPECT_EQ(res, expected);
}
TEST(fimpera_test_suite_finderec, negative_and_positive_stretch_5) {
    // be able to find a stretch and its abundance
    int K = 12;
    int z = 3;
    std::string query = "0123456789ABCDEFGHIJKL";
    std::vector<int> expected = {0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1};
    countingBF::CBF cbf = countingBF::CBF(1000, 5);
    cbf.set("89ABCDEFG");
    cbf.set("9ABCDEFGH");
    cbf.set("ABCDEFGHI");
    cbf.set("BCDEFGHIJ");
    cbf.set("CDEFGHIJK");
    cbf.set("DEFGHIJKL");

    std::vector<int> res = finderec(cbf, query, K, z, false);

    EXPECT_EQ(res, expected);
}
TEST(fimpera_test_suite_finderec, negative_and_positive_stretch_6) {
    // be able to find a stretch and its abundance
    int K = 12;
    int z = 3;
    std::string query = "0123456789ABCDEFGHIJKL";
    std::vector<int> expected = {0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1};
    countingBF::CBF cbf = countingBF::CBF(1000, 5);
    cbf.set("789ABCDEF");
    cbf.set("89ABCDEFG");
    cbf.set("9ABCDEFGH");
    cbf.set("ABCDEFGHI");
    cbf.set("BCDEFGHIJ");
    cbf.set("CDEFGHIJK");
    cbf.set("DEFGHIJKL");

    std::vector<int> res = finderec(cbf, query, K, z, false);

    EXPECT_EQ(res, expected);
}