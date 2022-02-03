#include <fimpera-lib/CBF.hpp>
#include <fimpera-lib/fimpera.hpp>
#include <iostream>

#include "args.hpp"

int main(int argc, char* argv[]) {
    argparse::ArgumentParser program("fimpera_index", "0.0.1");
    // mandatory arguments
    program.add_argument("input_filename").help("KMC file you want to index");
    program.add_argument("output_filename").help("output index name");
    program.add_argument("size").help("output index size").scan<'i', std::size_t>();
    // optional arguments
    program.add_argument("-K").help("size of Kmers").default_value(31).scan<'i', int>();
    program.add_argument("-z").help("value of z (cf paper of findere)").default_value(3).scan<'i', int>();
    program.add_argument("-b").help("number of bit per bucket in the output index").default_value(5).scan<'i', int>();
    program.add_argument("--canonical").help("will fimpera index canonical Kmers ?").default_value(false).implicit_value(true);

    parse(program, argc, argv);

    // create the filter
    fimpera<countingBF::CBF>
        f(program.get("input_filename"), program.get<int>("-K"), program.get<int>("-z"), program["--canonical"] == true, program.get<std::size_t>("size"), program.get<int>("-b"));
    // save it
    f.save(program.get("output_filename"));

    return 0;
}