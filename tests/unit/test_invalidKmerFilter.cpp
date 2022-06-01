#include <gtest/gtest.h>

#include <fimpera-lib/InvalidKmerFilter.hpp>
#include <string>
TEST(invalidKmerFilter, kmer_alone) {
    InvalidKmerFilter invalidKmerFilter = InvalidKmerFilter(10);
    const std::string kmer = "aaaaaaaaaa";

    bool isInvalid = invalidKmerFilter.isInvalid_init(kmer);
    float complexity = invalidKmerFilter.getComplexity();

    EXPECT_EQ(isInvalid, true);
    EXPECT_GE(complexity, 0.16);
    EXPECT_LE(complexity, 0.17);
}

TEST(invalidKmerFilter, kmer_alone2) {
    InvalidKmerFilter invalidKmerFilter = InvalidKmerFilter(10);
    const std::string kmer = "aacaaataaa";

    bool isInvalid = invalidKmerFilter.isInvalid_init(kmer);
    float complexity = invalidKmerFilter.getComplexity();

    EXPECT_EQ(isInvalid, false);
    EXPECT_GE(complexity, 0.99);
    EXPECT_LE(complexity, 1.01);
}