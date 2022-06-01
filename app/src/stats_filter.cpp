#include <robin_hood.h>

#include <fimpera-lib/CBF.hpp>
#include <fimpera-lib/evaluation/UnlimitedTruthInTheShapeOfAnAMQ.hpp>
#include <fimpera-lib/fimpera.hpp>

#include "args.hpp"

int main(int argc, char* argv[]) {
    argparse::ArgumentParser program("fimpera_index", "0.0.1");
    // mandatory arguments
    program.add_argument("filename").help("file you want to index");

    parse(program, argc, argv);

    const std::string index_filename = program.get("filename");

    fimpera<countingBF::CBF> f = fimpera<countingBF::CBF>(index_filename, 35, 5, false, 1000, 5);
    countingBF::CBF filter = f.getInnerFilter();
    auto [filter_vector_stats, size] = filter.getStats();
    std::cout << "abundance class; number of element of that class; total number of cells in the filter; percentage of cells allocated to that abundance" << std::endl;
    std::size_t i = 0;
    for (auto x : filter_vector_stats) {
        std::cout << i << " " << x << " " << size << " " << (float)x / size << std::endl;
        i++;
    }
}