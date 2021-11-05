#include <fimpera-lib/CBF.hpp>
#include <fimpera-lib/fimpera.hpp>

#include "args/infos_args.hpp"

int main(int argc, char* argv[]) {
    cxxopts::ParseResult arguments = parseArgvInfo(argc, argv);
    const auto& [input_filename] = getArgsInfo(arguments);

    std::ifstream fin(input_filename, std::ios::out | std::ofstream::binary);

    const auto& [uuid, description, k, z, canonical, jsonString] = getMetadata(fin);
    std::cout << "Reading index " << input_filename << ":" << std::endl;
    std::cout << "    " << description << std::endl;
    std::cout << "    uuid: " << uuid << std::endl;
    std::cout << "    k: " << k << std::endl;
    std::cout << "    z: " << z << std::endl;
}