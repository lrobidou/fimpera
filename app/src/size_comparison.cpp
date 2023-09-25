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


template<typename T>
double getAverage(std::vector<T> const& v) {
    if (v.empty()) {
        return 0;
    }
    return std::accumulate(v.begin(), v.end(), 0.0) / v.size();
}

template <typename Strategy>
void compareSize(const std::string& kmc_filename_kmers, const std::string& filename_query, std::size_t size_start, std::size_t size_stop, std::size_t size_step, int b, Strategy strategy) {
    checkExists(kmc_filename_kmers);
    checkExists(filename_query);
    std::vector<std::vector<std::vector<uint64_t>>> vect_mat_k_vs_truth;
    std::vector<fimpera<countingBF::CBF>> vect_fimpera_cbf;

    // it does not make any sens to give a size to
    // fimpera<LimitedTruthInTheShapeOfAnAMQ>, but it is required to give one
    // to comply the same interface as fimpera<countingBF::CBF>
    // let's just use a random number
    uint64_t nbBitTruth = 100;
    fimpera<LimitedTruthInTheShapeOfAnAMQ> truth_k = fimpera<LimitedTruthInTheShapeOfAnAMQ>(strategy, kmc_filename_kmers, 31, 0, true, nbBitTruth, b, 1);

    std::cout << "truth initialised" << std::endl;

    std::vector<std::size_t> sizes;
    for (std::size_t size = size_start; size < size_stop; size += size_step) {
        sizes.push_back(size);
        vect_fimpera_cbf.push_back(fimpera<countingBF::CBF>(strategy, kmc_filename_kmers, 31, 3, true, size, b, 1));
        vect_mat_k_vs_truth.push_back(create_matrix(256));
        std::cout << "cbf initialised (size = " << size << ")" << std::endl;
    }

    std::cout << "stop" << std::endl;
    for (const auto& [read, header] : fimpera_lib::generators::ReadReader(filename_query)) {
        std::vector<int> truth_response = filter(truth_k.queryRead(read));
        int avg_truth = std::round(getAverage(truth_response));

        for (std::size_t i=0; i < sizes.size(); i++) {
            // int size = sizes[i];
            std::vector<int> k_response = filter(vect_fimpera_cbf[i].queryRead(read));
            if ((truth_response.size() != 0)) {
                if (truth_response.size() != k_response.size()) {
                    std::cerr << "truth_response.size() != k_response.size()" << std::endl;
                }
            }
            int avg_k = std::round(getAverage(k_response));
            vect_mat_k_vs_truth[i][avg_truth][avg_k] += 1;
        }
    }

    for (std::size_t i = 0; i < sizes.size(); i++) {
        std::cout << "[matrix, size = "<<sizes[i]<<"]" << std::endl;
        print_matrix(vect_mat_k_vs_truth[i]);
        std::cout << "[end matrix]" << std::endl;
    }
}

int main(int argc, char* argv[]) {
    argparse::ArgumentParser program("fimpera_index", "0.0.1");
    // mandatory arguments
    program.add_argument("kmc_filename_kmers").help("kmer file you want to index");
    program.add_argument("filename_query").help("file you want to query against the index");
    program.add_argument("size_start").help("output index size").scan<'i', std::size_t>();
    program.add_argument("size_stop").help("output index size").scan<'i', std::size_t>();
    program.add_argument("size_step").help("output index size").scan<'i', std::size_t>();
    program.add_argument("b").help("the number of bits per buckets in the filter").scan<'i', int>();

    parse(program, argc, argv);

    const std::string kmc_filename_kmers = program.get("kmc_filename_kmers");
    const std::string filename_query = program.get("filename_query");
    const std::size_t size_start = program.get<std::size_t>("size_start");
    const std::size_t size_stop = program.get<std::size_t>("size_stop");
    const std::size_t size_step = program.get<std::size_t>("size_step");
    const int b = program.get<int>("b");
    abundanceToIdentifierStrategy::identity idStrategy = abundanceToIdentifierStrategy::identity();

    compareSize(kmc_filename_kmers,filename_query, size_start, size_stop, size_step, b, idStrategy);
}
