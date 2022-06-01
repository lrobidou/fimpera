#include <gtest/gtest.h>

#include <fimpera-lib/CBF.hpp>
#include <fimpera-lib/fimpera.hpp>

TEST(fimpera_test_suite_fimpera, fimpera_eq) {
    fimpera<countingBF::CBF> f1 = fimpera<countingBF::CBF>("../../tests/unit/data/1000LinesTest.txt", 35, 0, false, 10000, 5);  // random size
    fimpera<countingBF::CBF> f2 = fimpera<countingBF::CBF>("../../tests/unit/data/1000LinesTest.txt", 35, 0, false, 10000, 5);  // random size
    EXPECT_TRUE(f1 == f2);
}

TEST(fimpera_test_suite_fimpera, fimpera_not_eq_1) {
    fimpera<countingBF::CBF> f1 = fimpera<countingBF::CBF>("../../tests/unit/data/1000LinesTest.txt", 35, 0, false, 10000, 5);  // random size
    fimpera<countingBF::CBF> f2 = fimpera<countingBF::CBF>("../../tests/unit/data/1000LinesTest.txt", 35, 0, false, 10000, 1);  // random size
    EXPECT_FALSE(f1 == f2);
}

TEST(fimpera_test_suite_fimpera, fimpera_not_eq_2) {
    fimpera<countingBF::CBF> f1 = fimpera<countingBF::CBF>("../../tests/unit/data/1000LinesTest.txt", 35, 0, false, 10000, 5);  // random size
    fimpera<countingBF::CBF> f2 = fimpera<countingBF::CBF>("../../tests/unit/data/1000LinesTest.txt", 35, 1, false, 10000, 5);  // random size
    EXPECT_FALSE(f1 == f2);
}

TEST(fimpera_test_suite_fimpera, readUUID) {
    std::string filename = "../../tests/unit/data/test.idx";
    std::ifstream fin(filename, std::ios::out | std::ofstream::binary);
    EXPECT_EQ(readUUID(fin), "23e0132d-0e3d-4639-9794-69c2e22a0af4");
}

TEST(fimpera_test_suite_fimpera, getMetadatav0) {
    std::string filename = "../../tests/unit/data/test.idx";
    std::ifstream fin(filename, std::ios::out | std::ofstream::binary);

    readUUID(fin);  // skip the uuid

    const auto &[description, k, z, canonical, jsonString] = getMetadatav0(fin);

    EXPECT_EQ(description, "fimpera index version 0. https://github.com/lrobidou/fimpera");
    EXPECT_EQ(k, 32);
    EXPECT_EQ(z, 3);
    EXPECT_EQ(canonical, false);
    EXPECT_EQ(jsonString, "{\"strategy\":\"identity\"}");
}

TEST(fimpera_test_suite_fimpera, getMetadata) {
    std::string filename = "../../tests/unit/data/test.idx";
    std::ifstream fin(filename, std::ios::out | std::ofstream::binary);

    const auto &[uuid, description, k, z, canonical, jsonString] = getMetadata(fin);

    EXPECT_EQ(uuid, "23e0132d-0e3d-4639-9794-69c2e22a0af4");
    EXPECT_EQ(description, "fimpera index version 0. https://github.com/lrobidou/fimpera");
    EXPECT_EQ(k, 32);
    EXPECT_EQ(z, 3);
    EXPECT_EQ(canonical, false);
    EXPECT_EQ(jsonString, "{\"strategy\":\"identity\"}");
}

TEST(fimpera_test_suite_fimpera, getMetadataWrongUUID) {
    std::string filename = "../../tests/unit/data/testWrongUUID.idx";
    std::ifstream fin(filename, std::ios::out | std::ofstream::binary);

    const std::string msg = "Invalid uuid found when reading index. UUID is: 23e0132d-0e3d-4639-9794-69c2e22a0af5 .";
    try {
        const auto &[uuid, description, k, z, canonical, jsonString] = getMetadata(fin);
        FAIL() << "Expected std::runtime_error";
    } catch (const std::runtime_error &e) {
        EXPECT_EQ(e.what(), msg);
    } catch (...) {
        FAIL() << "Expected std::runtime_error";
    }
}