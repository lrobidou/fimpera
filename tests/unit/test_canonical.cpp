#include <gtest/gtest.h>

#include <fimpera-lib/canonical.hpp>

// TEST(fimpera_test_suite_canonical, test_complement) {
//     EXPECT_EQ('T', complement('A'));
//     EXPECT_EQ('A', complement('T'));
//     EXPECT_EQ('C', complement('G'));
//     EXPECT_EQ('G', complement('C'));
// }

// TEST(fimpera_test_suite_canonical, test_reverse_complement) {
//     const std::string seq = "GTGTTCATTAGCCAGTGAATTATCGCTACAAAGTAGATCCCAATCTTCTT";
//     const std::string expected = "AAGAAGATTGGGATCTACTTTGTAGCGATAATTCACTGGCTAATGAACAC";

//     std::string copy = seq;
//     reverseComplement(copy);
//     EXPECT_EQ(copy, expected);
// }

TEST(fimpera_test_suite_canonical, test_get_canonical) {
    const std::string seq = "GTGTTCATTAGCCAGTGAATTATCGCTACAAAGTAGATCCCAATCTTCTT";
    const std::string expected = "AAGAAGATTGGGATCTACTTTGTAGCGATAATTCACTGGCTAATGAACAC";

    EXPECT_EQ(toCanonical(seq), expected);
    EXPECT_EQ(toCanonical(expected), expected);
    EXPECT_EQ(toCanonical("AATT"), "AATT");
    EXPECT_EQ(toCanonical("TGTGGGTTTACTATTTTCTTACGCCACAACCAGAA"), "TGTGGGTTTACTATTTTCTTACGCCACAACCAGAA");
}

TEST(fimpera_test_suite_fimpera, cano0) {
    const std::string kmer = "AAACAGCGACGGAAGATAGCGTTGCTCCAGCAGTTG";
    const std::string revc = "CAACTGCTGGAGCAACGCTATCTTCCGTCGCTGTTT";
    EXPECT_EQ(toCanonical(kmer), kmer);
}

TEST(fimpera_test_suite_fimpera, can1) {
    const std::string kmer = "TTCAGCGACGGAAGATAGCGTTGCTCCAGCAGTTG";
    const std::string revc = "CAACTGCTGGAGCAACGCTATCTTCCGTCGCTGAA";
    EXPECT_EQ(toCanonical(kmer), revc);
}

// TEST(fimpera_test_suite_fimpera, make_can0) {
//     const std::string kmer = "AAACAGCGACGGAAGATAGCGTTGCTCCAGCAGTTG";
//     const std::string revc = "CAACTGCTGGAGCAACGCTATCTTCCGTCGCTGTTT";
//     EXPECT_EQ(make_canonical(kmer), kmer);
// }

// TEST(fimpera_test_suite_fimpera, make_can1) {
//     const std::string kmer = "TTCAGCGACGGAAGATAGCGTTGCTCCAGCAGTTG";
//     const std::string revc = "CAACTGCTGGAGCAACGCTATCTTCCGTCGCTGAA";
//     EXPECT_EQ(make_canonical(kmer), revc);
// }