#include <math.h>  // TODO remove ?
#include <robin_hood.h>

#include <chrono>
#include <fimpera-lib/CBF.hpp>
#include <fimpera-lib/evaluation/LimitedTruthInTheShapeOfAnAMQ.hpp>
#include <fimpera-lib/evaluation/UnlimitedTruthInTheShapeOfAnAMQ.hpp>
#include <fimpera-lib/fimpera.hpp>
#include <string>

#include "args.hpp"

inline void printv(const std::vector<int>& x) {
    for (std::size_t k = 0; k < x.size(); k++) {
        std::cout << x[k] << " ";
    }
    std::cout << std::endl;
}

/// Represents the exception for taking the median of an empty list
class median_of_empty_list_exception : public std::exception {
    virtual const char* what() const throw() {
        return "Attempt to take the median of an empty list of numbers.  "
               "The median of an empty list is undefined.";
    }
};

/// Return the median of a sequence of numbers defined by the random
/// access iterators begin and end.  The sequence must not be empty
///(median is undefined for an empty set).
///
/// The numbers must be convertible to double.
template <class RandAccessIter>
double median(RandAccessIter begin, RandAccessIter end) {
    if (begin == end) {
        throw median_of_empty_list_exception();
    }
    std::size_t size = end - begin;
    std::size_t middleIdx = size / 2;
    RandAccessIter target = begin + middleIdx;
    std::nth_element(begin, target, end);

    if (size % 2 != 0) {  // Odd number of elements
        return *target;
    } else {  // Even number of elements
        double a = *target;
        RandAccessIter targetNeighbor = target - 1;
        std::nth_element(begin, targetNeighbor, end);
        return (a + *targetNeighbor) / 2.0;
    }
}


template <
    class result_t = std::chrono::milliseconds,
    class clock_t = std::chrono::steady_clock,
    class duration_t = std::chrono::milliseconds>
auto since(std::chrono::time_point<clock_t, duration_t> const& start) {
    return std::chrono::duration_cast<result_t>(clock_t::now() - start);
}

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

template <typename Strategy>
void compareSmersAndKmers_in_cBF(const std::string& kmc_filename_kmers, const std::string& kmc_filename_smers, std::size_t size, int b, Strategy strategy, const std::string& filename_query) {
    bool _canonical = true;
    auto start = std::chrono::steady_clock::now();

    fimpera<LimitedTruthInTheShapeOfAnAMQ> truth_k = fimpera<LimitedTruthInTheShapeOfAnAMQ>(strategy, kmc_filename_kmers, 31, 0, true, size, b, 1);
    // fimpera<countingBF::CBF> cbf_k = fimpera<>(strategy, kmc_filename_kmers, 31, 0, true, size, b, 1);
    fimpera<countingBF::CBF> cbf_k = fimpera<countingBF::CBF>(strategy, kmc_filename_kmers, 31, 2, true, size, b, 1);
    fimpera<countingBF::CBF> cbf_s = fimpera<countingBF::CBF>(strategy, kmc_filename_smers, 29, 0, true, size, b, 1);
    cbf_s.increase_z_of(2);


    checkExists(kmc_filename_smers);
    checkExists(filename_query);

    std::ifstream myFileGz(kmc_filename_smers);
    zstr::istream myFile(myFileGz);

    std::string line;

    checkExists(filename_query);
    std::cout << "before matrix" << std::endl;

    std::vector<std::vector<uint64_t>> mat_k_vs_truth = create_matrix(256);
    std::vector<std::vector<uint64_t>> mat_s_vs_truth = create_matrix(256);
    std::vector<std::vector<uint64_t>> mat_s_vs_k = create_matrix(256);

    for (const auto& [read, header] : fimpera_lib::generators::ReadReader(filename_query)) {
        std::vector<int> truth_response = filter(truth_k.queryRead(read));
        std::vector<int> k_response = filter(cbf_k.queryRead(read));
        std::vector<int> s_response = filter(cbf_s.queryRead(read));

        if ((truth_response.size() != 0)) {
            if (truth_response.size() != s_response.size()) {
                std::cerr << "truth_response.size() != s_response.size()" << std::endl;
            }
            if (truth_response.size() != k_response.size()) {
                std::cerr << "truth_response.size() != k_response.size()" << std::endl;
            }
            int avg_truth = std::round(getAverage(truth_response));
            int avg_k = std::round(getAverage(k_response));
            int avg_s = std::round(getAverage(s_response));
            if (avg_truth > avg_s) {
                std::cout << "[debug] mk > ms\n";
                printv(truth_response);
                printv(s_response);
                printv(truth_k.querySmers(read));
                printv(cbf_s.querySmers(read));
                std::cout << "[end debug] mk > ms\n";
            }
            mat_k_vs_truth[avg_truth][avg_k] += 1;
            mat_s_vs_truth[avg_truth][avg_s] += 1;
            mat_s_vs_k[avg_k][avg_s] += 1;
            // mat_median[avg_truth][avg_s] += 1;
        }
    }
    std::cout << "[matrix]" << std::endl;
    print_matrix(mat_k_vs_truth);
    std::cout << "[end matrix]" << std::endl;

    std::cout << "[matrix]" << std::endl;
    print_matrix(mat_s_vs_truth);
    std::cout << "[end matrix]" << std::endl;

    std::cout << "[matrix]" << std::endl;
    print_matrix(mat_s_vs_k);
    std::cout << "[end matrix]" << std::endl;
}

int main(int argc, char* argv[]) {
    argparse::ArgumentParser program("fimpera_index", "0.0.1");
    // mandatory arguments
    // program.add_argument("input_filename").help("index you want to query");
    program.add_argument("kmc_filename_kmers").help("file you want to query against the index");
    program.add_argument("kmc_filename_smers").help("file you want to query against the index");
    program.add_argument("filename_query").help("query file");
    program.add_argument("size").help("output index size").scan<'i', std::size_t>();
    program.add_argument("b").help("the number of bits per buckets in the filter").scan<'i', int>();

    parse(program, argc, argv);

    const std::string kmc_filename_kmers = program.get("kmc_filename_kmers");
    const std::string kmc_filename_smers = program.get("kmc_filename_smers");
    const std::size_t size = program.get<std::size_t>("size");
    const int b = program.get<int>("b");
    const std::string filename_query = program.get("filename_query");
    abundanceToIdentifierStrategy::identity idStrategy = abundanceToIdentifierStrategy::identity();

    compareSmersAndKmers_in_cBF(kmc_filename_kmers, kmc_filename_smers, size, b, idStrategy, filename_query);
}
