#pragma once

#include "args.hpp"

cxxopts::ParseResult parseArgvQuery(int argc, char* argv[]) {
    cxxopts::Options options("index", "indexer of multifilter");
    options.add_options()                                                                                         //
        ("i,input", "input index", cxxopts::value<std::string>())                                                 //
        ("q,query", "name of the file that contains the query", cxxopts::value<std::string>())                    //
        ("K", "length of K-mers", cxxopts::value<unsigned long long>())                                           //
        ("z", "number of sub-k-mers per kmer", cxxopts::value<unsigned long long>())                              //
        ("h,help", "display this help", cxxopts::value<bool>()->default_value("false")->implicit_value("true"));  //
    // type
    // threshold
    // ("c,canonical", "do you want to index canonical Kmers ?", cxxopts::value<bool>()->default_value("false")->implicit_value("true"));
    return options.parse(argc, argv);
}

std::tuple<std::string, std::string, unsigned long long, unsigned long long> getArgsQuery(const cxxopts::ParseResult& arguments) {
    const bool displayHelp = getOneArg<bool>(arguments, "h");
    if (displayHelp) {
        std::cout << "fimpera_query: query Kmers using kmers (K>k) indexed in an Approximate Membership Query data structure (a counting Bloom filter in this implementation) stored on a disk." << std::endl;
        std::cout << "Usage:" << std::endl;
        std::cout << "    ./bin/fimpera_query -i inputFile -q queryFile [-K <K>] [-z <z>] [-c]" << std::endl;
        std::cout << "Options:" << std::endl;
        std::cout << "    -i: input index you want to query" << std::endl;  // TODO may be gzipped ?
        std::cout << "    -q: file to query against the index; may be gzipped" << std::endl;
        std::cout << "    -K: the value for K: size of the Kmers queried (default=value stored in the index)" << std::endl;
        std::cout << "    -z: the value for z. fimpera indexes kmers of size K-z. (default=value stored in the index)" << std::endl;
        // type
        // canonical
        // threshold

        std::cout << "Example:" << std::endl;
        // std::cout << "    ./bin/findere_index -i \"data/ecoli2.fasta\",\"data/ecoli3.fasta\",\"data/Listeria phage.fasta\",\"data/Penicillium chrysogenum.fasta\" -o indexedFastas -K 31 -z 3 -b 10000000 -t bio" << std::endl;  // TODO
        exit(0);
    }

    // mandatory args
    const std::string index_filename = getOneArg<std::string>(arguments, "i");
    const std::string query_filename = getOneArg<std::string>(arguments, "q");

    // optional args
    const unsigned long long K = getOneArgOptional<unsigned long long>(arguments, "K", 31);  // TODO
    const unsigned long long z = getOneArgOptional<unsigned long long>(arguments, "z", 3);   // TODO
    // const bool canonical = getOneArgOptional<bool>(arguments, "c", false);

    return {index_filename, query_filename, K, z};
}