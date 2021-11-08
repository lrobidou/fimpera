#include <fimpera-lib/CBF.hpp>
#include <fimpera-lib/fimpera.hpp>
#include <iostream>

#include "args/index_args.hpp"

int main(int argc, char* argv[]) {
    cxxopts::ParseResult arguments = parseArgvIndexer(argc, argv);
    const auto& [input_filename, output_filename, K, z, size_bits, nb_bit_per_bucket] = getArgsIndexer(arguments);

    fimpera<countingBF::CBF> f = fimpera<countingBF::CBF>(input_filename, K, z, false, size_bits, nb_bit_per_bucket);  //TODO canonical
    f.save(output_filename);

    return 0;
}