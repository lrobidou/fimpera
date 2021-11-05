#pragma once

#include "args.hpp"

cxxopts::ParseResult parseArgvIndexer(int argc, char* argv[]) {
    cxxopts::Options options("index", "indexer of multifilter");
    options.add_options()                                                             //
        ("i,input", "input file", cxxopts::value<std::string>())                      //
        ("o,output", "output filename for the BF", cxxopts::value<std::string>())     //
        ("K", "length of K-mers", cxxopts::value<unsigned long long>())               //
        ("z", "number of sub-k-mers per kmer", cxxopts::value<unsigned long long>())  //
        // ("t,type", "txt input files", cxxopts::value<std::string>())                                             //
        ("s,size", "size of the filter", cxxopts::value<unsigned long long>())                                           //
        ("b,bucketSize", "number of bit per bucket of the counting Bloom filter", cxxopts::value<unsigned long long>())  //
        ("h,help", "display this help", cxxopts::value<bool>()->default_value("false")->implicit_value("true"));         //
    // ("c,canonical", "do you want to index canonical Kmers ?", cxxopts::value<bool>()->default_value("false")->implicit_value("true"));
    return options.parse(argc, argv);
}

std::tuple<std::string, std::string, unsigned long long, unsigned long long, unsigned long long, unsigned long long> getArgsIndexer(const cxxopts::ParseResult& arguments) {
    const bool displayHelp = getOneArg<bool>(arguments, "h");
    if (displayHelp) {
        std::cout << "fimpera_index: index Kmers using kmers (K>k) indexed in an Approximate Membership Query data structure (a counting Bloom filter in this implementation)." << std::endl;
        std::cout << "Usage:" << std::endl;
        std::cout << "    ./bin/fimpera_index -i inputFile -o <index> -b <bits> [-K <K>] [-z <z>] [-c]" << std::endl;
        std::cout << "Options:" << std::endl;
        std::cout << "    -i: the input file you want to index." << std::endl;  // TODO may be gzipped ?
        std::cout << "    -o: output index name" << std::endl;
        std::cout << "    -K: the value for K: size of the Kmers queried by findere_query. Default=31" << std::endl;
        std::cout << "    -z: the value for z. findere indexes kmers of size K-z. Default=3" << std::endl;
        std::cout << "    -s: size (in bits) of the counting Bloom filter" << std::endl;
        std::cout << "    -b: the number of bit per bucket of the counting Bloom filter" << std::endl;

        // std::cout << "    -c: index canonical Kmers if type is bio." << std::endl;  // TODO ?
        std::cout << "Example:" << std::endl;
        // std::cout << "    ./bin/findere_index -i \"data/ecoli2.fasta\",\"data/ecoli3.fasta\",\"data/Listeria phage.fasta\",\"data/Penicillium chrysogenum.fasta\" -o indexedFastas -K 31 -z 3 -b 10000000 -t bio" << std::endl;  // TODO
        exit(0);
    }

    // mandatory args
    const std::string input_filename = getOneArg<std::string>(arguments, "i");
    const std::string output = getOneArg<std::string>(arguments, "o");
    const unsigned long long size_bits = getOneArg<unsigned long long>(arguments, "s");
    const unsigned long long nb_bit_per_bucket = getOneArg<unsigned long long>(arguments, "b");  //TODO make it 1 by default ?

    // optional args
    const unsigned long long K = getOneArgOptional<unsigned long long>(arguments, "K", 31);
    const unsigned long long z = getOneArgOptional<unsigned long long>(arguments, "z", 3);
    // const bool canonical = getOneArgOptional<bool>(arguments, "c", false);

    return {input_filename, output, K, z, size_bits, nb_bit_per_bucket};
}