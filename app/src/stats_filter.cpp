#include <fimpera-lib/CBF.hpp>
#include <fimpera-lib/fimpera.hpp>

#include "args.hpp"
// Heavily assumes that the filter contains a CBF
int main(int argc, char* argv[]) {
    argparse::ArgumentParser program("fimpera_index", "0.0.1");
    // mandatory arguments
    program.add_argument("index_filename").help("index file you want to index");

    parse(program, argc, argv);

    const std::string index_filename = program.get("index_filename");

    // std::ifstream fin(input_filename, std::ios::out | std::ofstream::binary);

    // const auto& [uuid, description, k, z, canonical, jsonString] = getMetadata(fin);
    // std::cout << "Reading index " << input_filename << ":" << std::endl;
    // std::cout << "    " << description << std::endl;
    // std::cout << "    uuid: " << uuid << std::endl;
    // std::cout << "    k: " << k << std::endl;
    // std::cout << "    z: " << z << std::endl;
    // if (jsonString.size() != 0) {
    //     std::cout << jsonString << std::endl;
    // }
    fimpera<countingBF::CBF> f = fimpera<countingBF::CBF>(index_filename);
    countingBF::CBF filter = f.getInnerFilter();
    for (auto x : filter.getStats()) {
        std::cout << x << " ";
    }
}