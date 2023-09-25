#include <chrono>
#include <fimpera-lib/CBF.hpp>
#include <fimpera-lib/evaluation/LimitedTruthInTheShapeOfAnAMQ.hpp>
#include <fimpera-lib/fimpera.hpp>
#include <string>

#include "args.hpp"

// figure S6 du papier


std::vector<std::vector<uint64_t>> create_matrix(std::size_t matrix_size) {
    std::vector<std::vector<uint64_t>> matrix(matrix_size);
    for (std::size_t i = 0; i < matrix_size; i++) {
        matrix[i] = std::vector<uint64_t>(matrix_size, 0);
    }
    return matrix;
}

void print_matrix(std::vector<std::vector<uint64_t>> matrix) {
    std::size_t matrix_size = matrix.size();
    for (std::size_t i = 0; i < matrix_size; i++) {
        for (std::size_t j = 0; j < matrix_size; j++) {
            std::cout << matrix[i][j] << " ";
        }
        std::cout << std::endl;
    }
}

// remove values <0 from v
std::vector<int> filter(const std::vector<int>& v) {
    std::vector<int> f;  // filtered
    f.reserve(v.size());
    for (const auto& x : v) {
        if (x >= 0) {
            f.push_back(x);
        }
    }
    return f;
}


template<typename T>
double getAverage(std::vector<T> const& v) {
    if (v.empty()) {
        return 0;
    }
    return std::accumulate(v.begin(), v.end(), 0.0) / v.size();
}

void hash_fct_comparison(const std::string& kmc_filename_kmers, std::size_t size, int b, const std::string& filename_query) {
    abundanceToIdentifierStrategy::identity strategy = abundanceToIdentifierStrategy::identity();
    bool canonical = true;
    const unsigned int z = 3;

    fimpera<LimitedTruthInTheShapeOfAnAMQ> truth_k = fimpera<LimitedTruthInTheShapeOfAnAMQ>(strategy, kmc_filename_kmers, 31, 0, canonical, size, b, 1);
    fimpera<countingBF::CBF> cbf_z = fimpera<countingBF::CBF>(strategy, kmc_filename_kmers, 31, z, canonical, size, b, 1);
    fimpera<countingBF::CBF> cbf_h = fimpera<countingBF::CBF>(strategy, kmc_filename_kmers, 31, 0, canonical, size, b, z+1);

    checkExists(filename_query);

    std::string line;
    std::cout << "b = " << b << std::endl; 
    std::cout << "size = " << size << std::endl;
    std::cout << "z = " << z << std::endl;
    std::cout << "before matrix" << std::endl;
    std::vector<std::vector<uint64_t>> mat_z_vs_truth = create_matrix(256);
    std::vector<std::vector<uint64_t>> mat_0_vs_truth = create_matrix(256);
    std::vector<std::vector<uint64_t>> mat_z_vs_hash = create_matrix(256);

    for (const auto& [read, header] : fimpera_lib::generators::ReadReader(filename_query)) {
        std::vector<int> truth_response = filter(truth_k.queryRead(read));
        std::vector<int> response_z = filter(cbf_z.queryRead(read));
        std::vector<int> response_h = filter(cbf_h.queryRead(read));

        if ((truth_response.size() != 0)) {
            if (truth_response.size() != response_z.size()) {
                std::cerr << "truth_response.size() != k_response.size()" << std::endl;
            }
            if (truth_response.size() != response_h.size()) {
                std::cerr << "truth_response.size() != h_response.size()" << std::endl;
            }

            int avg_truth = std::round(getAverage(truth_response));
            int avg_z = std::round(getAverage(response_z));
            int avg_h = std::round(getAverage(response_h));
            mat_z_vs_truth[avg_truth][avg_z] += 1;
            mat_0_vs_truth[avg_truth][avg_h] += 1;
            mat_z_vs_hash[avg_h][avg_z] += 1;
        }
    }

    std::cout << "[matrix fimpera vs truth]" << std::endl;
    print_matrix(mat_z_vs_truth);
    std::cout << "[end matrix]" << std::endl;

    std::cout << "[matrix hash vs truth]" << std::endl;
    print_matrix(mat_0_vs_truth);
    std::cout << "[end matrix]" << std::endl;

    std::cout << "[matrix fimpera vs hash]" << std::endl;
    print_matrix(mat_z_vs_hash);
    std::cout << "[end matrix]" << std::endl;
}

int main(int argc, char* argv[]) {
    argparse::ArgumentParser program("fimpera_index", "0.0.1");
    // mandatory arguments
    // program.add_argument("input_filename").help("index you want to query");
    program.add_argument("kmc_filename_kmers").help("file you want to query against the index");
    program.add_argument("filename_query").help("query file");
    program.add_argument("size").help("output index size").scan<'i', std::size_t>();
    program.add_argument("b").help("the number of bits per buckets in the filter").scan<'i', int>();

    parse(program, argc, argv);

    const std::string kmc_filename_kmers = program.get("kmc_filename_kmers");
    const std::size_t size = program.get<std::size_t>("size");
    const int b = program.get<int>("b");
    const std::string filename_query = program.get("filename_query");
    

    hash_fct_comparison(kmc_filename_kmers, size, b, filename_query);
}
