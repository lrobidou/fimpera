#include <fimpera-lib/CBF.hpp>
#include <fimpera-lib/abundanceToIdentifierStrategy.hpp>
#include <fimpera-lib/fimpera.hpp>
#include <iostream>

#include "args.hpp"

#include <iostream>
#include <chrono>
#include <ctime> 

int main(int argc, char* argv[]) {
    argparse::ArgumentParser program("fimpera_index", "0.0.1");
    // mandatory arguments
    program.add_argument("input_filename").help("KMC file you want to index");
    program.add_argument("output_filename").help("output index name");
    program.add_argument("size").help("output index size").scan<'i', std::size_t>();
    // optional arguments
    program.add_argument("-k").help("size of kmers").default_value(31).scan<'i', int>();
    program.add_argument("-z").help("value of z (cf paper of findere)").default_value(3).scan<'i', int>();
    program.add_argument("-b").help("number of bit per bucket in the output index").default_value(5).scan<'i', int>();
    program.add_argument("--canonical").help("will fimpera index canonical kmers ?").default_value(false).implicit_value(true);

    parse(program, argc, argv);
    // create the filter

    abundanceToIdentifierStrategy::identity idStrategy = abundanceToIdentifierStrategy::identity();
    abundanceToIdentifierStrategy::log2 logStrategy = abundanceToIdentifierStrategy::log2();

    auto start = std::chrono::system_clock::now();
    
    fimpera<countingBF::CBF>
        f(program.get("input_filename"), program.get<int>("-k"), program.get<int>("-z"), program["--canonical"] == true, program.get<std::size_t>("size"), program.get<int>("-b"));
    
    auto half = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed_seconds_index = half-start;
    std::cout << "time to index = " << elapsed_seconds_index.count() << std::endl;
    
    // save it
    f.save(program.get("output_filename"));
    
    auto end = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed_seconds_dump = end-start;
    std::cout << "time to index and dump = " << elapsed_seconds_dump.count() << std::endl;
    return 0;
}
