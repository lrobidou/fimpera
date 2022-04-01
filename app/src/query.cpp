#include <fimpera-lib/CBF.hpp>
#include <fimpera-lib/ResultGetter.hpp>
#include <fimpera-lib/fimpera.hpp>

#include "args.hpp"

int main(int argc, char* argv[]) {
    argparse::ArgumentParser program("fimpera_index", "0.0.1");

    program.add_argument("input_filename").help("index you want to query");
    program.add_argument("query_filename").help("file you want to query against the index");

    parse(program, argc, argv);

    const std::string index_filename = program.get("input_filename");
    const std::string query_filename = program.get("query_filename");

    fimpera<countingBF::CBF> f = fimpera<countingBF::CBF>(index_filename);
    ResultGetter result_getter = ResultGetter();
    f.query(query_filename, result_getter);
    const auto& [response, reads] = result_getter.getResult();
}