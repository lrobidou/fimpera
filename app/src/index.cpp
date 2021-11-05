#include <assert.h>  //TODO remove

#include <fimpera-lib/CBF.hpp>
#include <fimpera-lib/fimpera.hpp>
#include <iostream>

#include "args/index_args.hpp"

void index(const std::string& filename, const unsigned int& K, const unsigned int& z, uint64_t size, uint64_t nbBuckets, const std::string& destination) {
    fimpera<countingBF::CBF> f = fimpera<countingBF::CBF>(filename, K, z, false, size, nbBuckets);  //TODO canonical
    f.save(destination);
}

int main(int argc, char* argv[]) {
    cxxopts::ParseResult arguments = parseArgvIndexer(argc, argv);
    const auto& [input_filename, output, K, z, size_bits, nb_bit_per_bucket] = getArgsIndexer(arguments);

    // uint64_t nbBuckets = 1;
    // uint64_t size = 900581956 * nbBuckets;  // 19495706 -> 5%
    // int K = 35;
    // int z = 0;  // bucket 1 0.000467051%,  5: 0.000467051%
    // std::string fileToIndex = "/home/lrobidou/Documents/programmes/forge/fimperaBF/data/35merlists.txt";
    // std::string destination = "destination.idx";
    // std::string queryFile = "/home/lrobidou/Documents/programmes/forge/fimperaBF/data/bio/ecoli1.fasta";

    index(input_filename, K, z, size_bits, nb_bit_per_bucket, output);

    // char[] msg = "bonjour";
    // std::cout << strlen(msg) << std::endl;

    return 0;
}