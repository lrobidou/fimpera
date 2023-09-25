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

/**
 * Produces the last line of the Table 2 of the fimpera paper.
 * For a fixed k, how does z influence the speed of the query ?
 * At fixed k, s will decrease.
 */

inline void printv(const std::vector<int>& x) {
    for (std::size_t k = 0; k < x.size(); k++) {
        std::cout << x[k] << " ";
    }
    std::cout << std::endl;
}

inline void add(robin_hood::unordered_map<int, int>& map, const int& key, const int& valueToAdd) {
    robin_hood::unordered_map<int, int>::const_iterator got = map.find(key);
    if (got != map.end()) {                   // found
        int val = got->second;                // sauvegarde de la valeur
        map.erase(key);                       // suppression de la valeur dans la map
        map.insert({key, val + valueToAdd});  // insertion de la nouvelle valeur
    } else {                                  // not found
        map.insert({key, valueToAdd});
    }
}

inline void push(robin_hood::unordered_map<int, std::vector<int>>& map, const int& key, const int& valueToAdd) {
    robin_hood::unordered_map<int, std::vector<int>>::const_iterator got = map.find(key);
    if (got != map.end()) {                  // found
        std::vector<int> val = got->second;  // sauvegarde de la valeur
        val.push_back(valueToAdd);
        map.erase(key);          // suppression de la valeur dans la map
        map.insert({key, val});  // insertion de la nouvelle valeur
    } else {                     // not found
        std::vector<int> toBeInserted;
        toBeInserted.push_back(valueToAdd);
        map.insert({key, toBeInserted});
    }
}

inline int absDiff(const int& a, const int& b) {
    int abs_diff = (a > b) ? (a - b) : (b - a);
    return abs_diff;
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

std::tuple<std::size_t, std::size_t, std::size_t, std::size_t> compareVectors(const std::vector<int>& result, const std::vector<int>& truth) {
    std::size_t tp = 0;
    std::size_t tn = 0;
    std::size_t fp = 0;
    std::size_t fn = 0;

    assert(result.size() == truth.size());

    for (std::size_t i = 0; i < result.size(); i++) {
        if (truth[i]) {
            if (result[i]) {
                tp++;
            } else {
                fn++;
            }
        } else {
            if (result[i]) {
                fp++;
            } else {
                tn++;
            }
        }
    }
    return {tp, tn, fp, fn};
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

template <typename T>
double getAverage(std::vector<T> const& v) {
    if (v.empty()) {
        return 0;
    }
    return std::accumulate(v.begin(), v.end(), 0.0) / v.size();
}

template <typename Strategy>
void getSpeed(const std::string& kmc_filename_31_mers, const std::string& kmc_filename_28_mers, const std::string& kmc_filename_26_mers, const std::string& kmc_filename_24_mers, const std::string& kmc_filename_22_mers, const std::string& kmc_filename_11_mers, std::size_t size_cbf, const int k, const int b, Strategy strategy, const std::string& query_filename) {
    checkExists(kmc_filename_31_mers);
    checkExists(kmc_filename_28_mers);
    checkExists(kmc_filename_26_mers);
    checkExists(kmc_filename_24_mers);
    checkExists(kmc_filename_22_mers);
    checkExists(kmc_filename_11_mers);
    checkExists(query_filename);

    std::cout << "every file exixts" << std::endl;

    {
        fimpera<countingBF::CBF> fimpera_31_mers(strategy, kmc_filename_31_mers, k, 0, true, size_cbf, b, 1);
        std::cout << "fimpera_31_mers built" << std::endl;
        ResultGetterEval result_getter = ResultGetterEval();
        auto start = std::chrono::steady_clock::now();
        fimpera_31_mers.query(query_filename, result_getter);
        std::cout << "fimpera_31_mers_query(ms)=" << since(start).count() << std::endl;
    }
    {
        fimpera<countingBF::CBF> fimpera_28_mers(strategy, kmc_filename_31_mers, k, 3, true, size_cbf, b, 1);
        std::cout << "fimpera_28_mers built" << std::endl;
        ResultGetterEval result_getter = ResultGetterEval();
        auto start = std::chrono::steady_clock::now();
        fimpera_28_mers.query(query_filename, result_getter);
        std::cout << "fimpera_28_mers_query(ms)=" << since(start).count() << std::endl;
    }
    {
        fimpera<countingBF::CBF> fimpera_26_mers(strategy, kmc_filename_31_mers, k, 5, true, size_cbf, b, 1);
        std::cout << "fimpera_26_mers built" << std::endl;
        ResultGetterEval result_getter = ResultGetterEval();
        auto start = std::chrono::steady_clock::now();
        fimpera_26_mers.query(query_filename, result_getter);
        std::cout << "fimpera_26_mers_query(ms)=" << since(start).count() << std::endl;
    }
    {
        fimpera<countingBF::CBF> fimpera_24_mers(strategy, kmc_filename_31_mers, k, 7, true, size_cbf, b, 1);
        std::cout << "fimpera_24_mers built" << std::endl;
        ResultGetterEval result_getter = ResultGetterEval();
        auto start = std::chrono::steady_clock::now();
        fimpera_24_mers.query(query_filename, result_getter);
        std::cout << "fimpera_24_mers_query(ms)=" << since(start).count() << std::endl;
    }
    {
        fimpera<countingBF::CBF> fimpera_22_mers(strategy, kmc_filename_31_mers, k, 9, true, size_cbf, b, 1);
        std::cout << "fimpera_22_mers built" << std::endl;
        ResultGetterEval result_getter = ResultGetterEval();
        auto start = std::chrono::steady_clock::now();
        fimpera_22_mers.query(query_filename, result_getter);
        std::cout << "fimpera_22_mers_query(ms)=" << since(start).count() << std::endl;
    }
    {
        fimpera<countingBF::CBF> fimpera_11_mers(strategy, kmc_filename_31_mers, k, 20, true, size_cbf, b, 1);
        std::cout << "fimpera_11_mers built" << std::endl;
        ResultGetterEval result_getter = ResultGetterEval();
        auto start = std::chrono::steady_clock::now();
        fimpera_11_mers.query(query_filename, result_getter);
        std::cout << "fimpera_11_mers_query(ms)=" << since(start).count() << std::endl;
    }
}

int main(int argc, char* argv[]) {
    argparse::ArgumentParser program("fimpera_index", "0.0.1");
    // mandatory arguments
    // program.add_argument("kmc_filename_kmers").help("kmer file you want to index");
    program.add_argument("kmc_filename_31-mers").help("kmer file you want to index");  // z = 0
    program.add_argument("kmc_filename_28-mers").help("kmer file you want to index");  // z = 3
    program.add_argument("kmc_filename_26-mers").help("kmer file you want to index");  // z = 5
    program.add_argument("kmc_filename_24-mers").help("kmer file you want to index");  // z = 7
    program.add_argument("kmc_filename_22-mers").help("kmer file you want to index");  // z = 9
    program.add_argument("kmc_filename_11-mers").help("kmer file you want to index");  // z = 20
    program.add_argument("filename_query").help("file you want to query against the index");
    program.add_argument("size_cbf").help("output index size").scan<'i', std::size_t>();
    program.add_argument("k").help("k").scan<'i', int>();
    program.add_argument("b").help("the number of bits per buckets in the filter").scan<'i', int>();

    parse(program, argc, argv);

    // const std::string kmc_filename_kmers = program.get("kmc_filename_kmers");
    const std::string kmc_filename_31_mers = program.get("kmc_filename_31-mers");
    const std::string kmc_filename_28_mers = program.get("kmc_filename_28-mers");
    const std::string kmc_filename_26_mers = program.get("kmc_filename_26-mers");
    const std::string kmc_filename_24_mers = program.get("kmc_filename_24-mers");
    const std::string kmc_filename_22_mers = program.get("kmc_filename_22-mers");
    const std::string kmc_filename_11_mers = program.get("kmc_filename_11-mers");
    // 3480000000.0
    const std::string query_filename = program.get("filename_query");

    const std::size_t size_cbf = program.get<std::size_t>("size_cbf");
    const int k = program.get<int>("k");
    const int b = program.get<int>("b");

    abundanceToIdentifierStrategy::log2 logStrategy = abundanceToIdentifierStrategy::log2();
    getSpeed(kmc_filename_31_mers, kmc_filename_28_mers, kmc_filename_26_mers, kmc_filename_24_mers, kmc_filename_22_mers, kmc_filename_11_mers, size_cbf, k, b, logStrategy, query_filename);
}
