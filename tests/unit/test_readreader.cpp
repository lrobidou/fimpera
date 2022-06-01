#include <gtest/gtest.h>

#include <fimpera-lib/generators/ReadReader.hpp>

TEST(fimpera_test_suite_readreader, readreader_fa) {
    const std::vector<std::string> reads = {
        "TACAATGAAGAACTCAATCGTATGCCGTCTTTTGTT",
        "GACATCACATGCTGGGTCGTATGCCGTCTTTTTCTT",
        "TTACTCTTTTTGTTTATTGTTTTCCGTCTTTTTGTT",
        "TTCTGGTGTTCACTGTTTCTTTTGCCGTCTTTTGTT",
        "TACACCGGACACGGGATTGGATGCCCTCTTTTTGTT"};

    const std::vector<std::string> headers = {
        ">SRR039001.1 CRG080910-3E6:8:1:522:318 length=36",
        ">SRR039001.2 CRG080910-3E6:8:1:351:650 length=36",
        ">SRR039001.3 CRG080910-3E6:8:1:754:111 length=36",
        ">SRR039001.4 CRG080910-3E6:8:1:804:412 length=36",
        ">SRR039001.5 CRG080910-3E6:8:1:847:96 length=36"};

    int i = 0;
    for (const auto& [read, header] : fimpera_lib::generators::ReadReader("../../tests/unit/data/rnaseq_shortened.fasta")) {
        EXPECT_EQ(read, reads[i]);
        EXPECT_EQ(header, headers[i]);
        i++;
    }
}