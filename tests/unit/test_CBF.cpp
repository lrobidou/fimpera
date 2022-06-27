#include <gtest/gtest.h>

#include <fimpera-lib/CBF.hpp>

inline std::string random_string(size_t length) {
    auto randchar = []() -> char {
        const char charset[] =
            "0123456789"
            "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
            "abcdefghijklmnopqrstuvwxyz";
        const size_t max_index = (sizeof(charset) - 1);
        return charset[rand() % max_index];
    };
    std::string str(length, 0);
    std::generate_n(str.begin(), length, randchar);
    return str;
}
TEST(fimpera_test_suite_CBF, one_set) {
    uint64_t occurrence = 1;
    countingBF::CBF cbf = countingBF::CBF(1000, 5);
    cbf.set("123456789", occurrence);
    EXPECT_EQ(cbf.get("123456789"), occurrence);
}

TEST(fimpera_test_suite_CBF, multiple_set) {
    uint64_t occurrence = 1;
    countingBF::CBF cbf = countingBF::CBF(1000, 5);
    cbf.set("123456789", occurrence);
    cbf.set("123456789", occurrence);
    cbf.set("123456789", occurrence);
    cbf.set("123456789", occurrence);
    cbf.set("123456789", occurrence);
    EXPECT_EQ(cbf.get("123456789"), occurrence);
}

TEST(fimpera_test_suite_CBF, mutilple_set_check_max) {
    countingBF::CBF cbf = countingBF::CBF(1000, 5);
    uint64_t expected = 8;
    cbf.set("123456789", 1);
    cbf.set("123456789", 2);
    cbf.set("123456789", 8);
    cbf.set("123456789", 3);
    cbf.set("123456789", 1);
    EXPECT_EQ(cbf.get("123456789"), expected);
}

TEST(fimpera_test_suite_CBF, mutilple_set_check_max_limit) {
    countingBF::CBF cbf = countingBF::CBF(1000, 5);
    uint64_t maxLimit = 31;
    cbf.set("123456789", 1);
    cbf.set("123456789", 2);
    cbf.set("123456789", 8);
    cbf.set("123456789", 100);
    cbf.set("123456789", 1);
    EXPECT_EQ(cbf.get("123456789"), maxLimit);
}

// TODO write a function to get fpr (and test it)
// formula to get fpr (1-(1-1/m)**(n*k))**k
// here:
// n = nbElemnInsertion
// m = size
// k = 1 (not the size of kmers, the number of hash functions!)
TEST(fimpera_test_suite_CBF, fpr_1_hash_1) {
    uint64_t size = 194957;
    countingBF::CBF cbf = countingBF::CBF(size, 1);
    int nbElemnInsertion = 10000;
    int nbElemnTest = 100000;
    int k = 30;
    for (int i = 0; i < nbElemnInsertion; i++) {
        std::string s = random_string(k);
        cbf.set(s, 1);
    }
    int pos = 0;
    int neg = 0;
    for (int i = 0; i < nbElemnTest; i++) {
        std::string s = random_string(k);
        if (cbf.get(s) == 0) {
            neg++;
        } else {
            pos++;
        }
    }

    double fpr = (double)pos / (double)(pos + neg);
    EXPECT_EQ(pos + neg, nbElemnTest);
    EXPECT_GE(fpr, 0.048);
    EXPECT_LE(fpr, 0.052);
}

TEST(fimpera_test_suite_CBF, fpr_1_hash_4) {
    int nbHash = 4;
    countingBF::CBF cbf = countingBF::CBF(194957, 1, nbHash);
    int nbElemnInsertion = 10000;
    int nbElemnTest = 100000;
    int k = 30;
    for (int i = 0; i < nbElemnInsertion; i++) {
        std::string s = random_string(k);
        cbf.set(s, 1);
    }
    int pos = 0;
    int neg = 0;
    for (int i = 0; i < nbElemnTest; i++) {
        std::string s = random_string(k);
        if (cbf.get(s) == 0) {
            neg++;
        } else {
            pos++;
        }
    }

    double fpr = (double)pos / (double)(pos + neg);
    EXPECT_EQ(pos + neg, nbElemnTest);
    // in theory, FPR is about 0.00118
    EXPECT_GE(fpr, 0.00096);
    EXPECT_LE(fpr, 0.0014);
}

TEST(fimpera_test_suite_CBF, fpr_1_hash_15) {
    int nbHash = 15;
    countingBF::CBF cbf = countingBF::CBF(194957, 1, nbHash);
    int nbElemnInsertion = 10000;
    int nbElemnTest = 100000;
    int k = 30;
    for (int i = 0; i < nbElemnInsertion; i++) {
        std::string s = random_string(k);
        cbf.set(s, 1);
    }
    int pos = 0;
    int neg = 0;
    for (int i = 0; i < nbElemnTest; i++) {
        std::string s = random_string(k);
        if (cbf.get(s) == 0) {
            neg++;
        } else {
            pos++;
        }
    }

    double fpr = (double)pos / (double)(pos + neg);
    EXPECT_EQ(pos + neg, nbElemnTest);
    // in theory, FPR is about 8.832775369727666e-05
    EXPECT_GE(fpr, 0);
    EXPECT_LE(fpr, 0.00012);
}

TEST(fimpera_test_suite_CBF, fpr_n_hash_1) {
    int nbBuckets = 5;
    countingBF::CBF cbf = countingBF::CBF(194957 * nbBuckets, nbBuckets);
    int nbElemnInsertion = 10000;
    int nbElemnTest = 100000;
    int k = 30;
    for (int i = 0; i < nbElemnInsertion; i++) {
        std::string s = random_string(k);
        cbf.set(s, 1);
    }
    int pos = 0;
    int neg = 0;
    for (int i = 0; i < nbElemnTest; i++) {
        std::string s = random_string(k);
        if (cbf.get(s) == 0) {
            neg++;
        } else {
            pos++;
        }
    }
    double fpr = (double)pos / (double)(pos + neg);
    EXPECT_EQ(pos + neg, nbElemnTest);
    EXPECT_GE(fpr, 0.048);
    EXPECT_LE(fpr, 0.052);
}

TEST(fimpera_test_suite_CBF, fpr_n_hash_4) {
    int nbBuckets = 5;
    uint32_t nbHash = 4;
    countingBF::CBF cbf = countingBF::CBF(194957 * nbBuckets, nbBuckets, nbHash);
    int nbElemnInsertion = 10000;
    int nbElemnTest = 100000;
    int k = 30;
    for (int i = 0; i < nbElemnInsertion; i++) {
        std::string s = random_string(k);
        cbf.set(s, 1);
    }
    int pos = 0;
    int neg = 0;
    for (int i = 0; i < nbElemnTest; i++) {
        std::string s = random_string(k);
        if (cbf.get(s) == 0) {
            neg++;
        } else {
            pos++;
        }
    }
    double fpr = (double)pos / (double)(pos + neg);
    EXPECT_EQ(pos + neg, nbElemnTest);
    EXPECT_GE(fpr, 0.00112);
    EXPECT_LE(fpr, 0.00124);
}