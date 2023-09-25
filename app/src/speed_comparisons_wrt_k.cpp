#include <math.h>  // TODO remove ?
#include <robin_hood.h>

#include <chrono>
#include <fimpera-lib/CBF.hpp>
#include <fimpera-lib/evaluation/LimitedTruthInTheShapeOfAnAMQ.hpp>
#include <fimpera-lib/evaluation/ResultGetterEval.hpp>
#include <fimpera-lib/evaluation/UnlimitedTruthInTheShapeOfAnAMQ.hpp>
#include <fimpera-lib/fimpera.hpp>
#include <string>

#include "args.hpp"

template <
    class result_t = std::chrono::milliseconds,
    class clock_t = std::chrono::steady_clock,
    class duration_t = std::chrono::milliseconds>
auto since(std::chrono::time_point<clock_t, duration_t> const& start) {
    return std::chrono::duration_cast<result_t>(clock_t::now() - start);
}

template <typename Strategy>
void getSpeed(const std::string& kmc_filename_28_mers, std::size_t size_cbf, const int k, const int b, Strategy strategy, const std::string& query_filename) {
    checkExists(kmc_filename_28_mers);
    checkExists(query_filename);

    std::cout << "every file exixts" << std::endl;

    fimpera<countingBF::CBF> fimpera_28_mers(strategy, kmc_filename_28_mers, k, 0, true, size_cbf, b, 1);
    std::cout << "index done" << std::endl;

    {
        ResultGetterEval result_getter = ResultGetterEval();
        auto start = std::chrono::steady_clock::now();
        fimpera_28_mers.query(query_filename, result_getter);
        std::cout << "z = 0 => query time = " << since(start).count() << std::endl;
    }
    fimpera_28_mers.increase_z_of(1);
    {
        ResultGetterEval result_getter = ResultGetterEval();
        auto start = std::chrono::steady_clock::now();
        fimpera_28_mers.query(query_filename, result_getter);
        std::cout << "z = 1 => query time = " << since(start).count() << std::endl;
    }
    fimpera_28_mers.increase_z_of(1);
    {
        ResultGetterEval result_getter = ResultGetterEval();
        auto start = std::chrono::steady_clock::now();
        fimpera_28_mers.query(query_filename, result_getter);
        std::cout << "z = 2 => query time = " << since(start).count() << std::endl;
    }
    fimpera_28_mers.increase_z_of(1);
    {
        ResultGetterEval result_getter = ResultGetterEval();
        auto start = std::chrono::steady_clock::now();
        fimpera_28_mers.query(query_filename, result_getter);
        std::cout << "z = 3 => query time = " << since(start).count() << std::endl;
    }
    fimpera_28_mers.increase_z_of(3);
    {
        ResultGetterEval result_getter = ResultGetterEval();
        auto start = std::chrono::steady_clock::now();
        fimpera_28_mers.query(query_filename, result_getter);
        std::cout << "z = 6 => query time = " << since(start).count() << std::endl;
    }
    fimpera_28_mers.increase_z_of(3);
    {
        ResultGetterEval result_getter = ResultGetterEval();
        auto start = std::chrono::steady_clock::now();
        fimpera_28_mers.query(query_filename, result_getter);
        std::cout << "z = 9 => query time = " << since(start).count() << std::endl;
    }
    fimpera_28_mers.increase_z_of(6);
    {
        ResultGetterEval result_getter = ResultGetterEval();
        auto start = std::chrono::steady_clock::now();
        fimpera_28_mers.query(query_filename, result_getter);
        std::cout << "z = 15 => query time = " << since(start).count() << std::endl;
    }
}

int main(int argc, char* argv[]) {
    argparse::ArgumentParser program("fimpera_index", "0.0.1");
    // mandatory arguments
    program.add_argument("kmc_filename_28-mers").help("kmer file you want to index");
    program.add_argument("filename_query").help("file you want to query against the index");
    program.add_argument("size_cbf").help("output index size").scan<'i', std::size_t>();
    program.add_argument("k").help("k").scan<'i', int>();
    program.add_argument("b").help("the number of bits per buckets in the filter").scan<'i', int>();

    parse(program, argc, argv);
    const std::string kmc_filename_28_mers = program.get("kmc_filename_28-mers");
    // 3480000000.0
    const std::string query_filename = program.get("filename_query");

    const std::size_t size_cbf = program.get<std::size_t>("size_cbf");
    const int k = program.get<int>("k");
    const int b = program.get<int>("b");

    abundanceToIdentifierStrategy::log2 logStrategy = abundanceToIdentifierStrategy::log2();
    getSpeed(kmc_filename_28_mers, size_cbf, k, b, logStrategy, query_filename);
}
