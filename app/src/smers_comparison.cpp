#include <math.h>  // TODO remove ?
#include <robin_hood.h>

#include <chrono>
#include <fimpera-lib/CBF.hpp>
#include <fimpera-lib/evaluation/LimitedTruthInTheShapeOfAnAMQ.hpp>
#include <fimpera-lib/evaluation/UnlimitedTruthInTheShapeOfAnAMQ.hpp>
#include <fimpera-lib/fimpera.hpp>
#include <string>

#include "args.hpp"
/*
Computes a matrix such that

    |   x x x x x x
    |   x x x x x x
(1) |   x x x x x x
    |   x x x x x x
    |   x x x x x x
    |
    |-------------->
    v       (2)

 1: unlimited truth
    limited truth
    index
    unlimited ctruth
    limited ctruth
    index z
 2: unlimited truth
*/
inline void printv(const std::vector<int>& x) {
    for (int k = 0; k < x.size(); k++) {
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
void compareSmersAndKmers_in_cBF(const std::string& kmc_filename_kmers, const std::string& kmc_filename_smers, std::size_t size, int b, Strategy strategy, const std::string& filename_query) {
    bool _canonical = true;
    auto start = std::chrono::steady_clock::now();

    fimpera<countingBF::CBF> cbf_k = fimpera<countingBF::CBF>(strategy, kmc_filename_kmers, 31, 2, true, size, b, 1);  // on stocke des 29-mers

    fimpera<countingBF::CBF> cbf_s = fimpera<countingBF::CBF>(strategy, kmc_filename_smers, 29, 0, true, size, b, 1);  // on stocke des 29-mers

    checkExists(kmc_filename_smers);
    checkExists(filename_query);

    std::ifstream myFileGz(kmc_filename_smers);
    zstr::istream myFile(myFileGz);

    std::string line;
    //while (std::getline(myFile, line)) {  // \n are removed
    //    std::string smer;
    //    std::string abundanceStr;
    //    std::stringstream linestream(line);
    //    std::getline(linestream, smer, '\t');
    //    std::getline(linestream, abundanceStr, '\t');

    //    if (smer.size() != 29) {
    //        std::cerr << "smer.size() != 29" << std::endl;
    //    }

    //    try {
    //        uint64_t abundance = std::stoll(abundanceStr);
    //        uint64_t id = strategy.fct(abundance);
    //        if (_canonical) {
    //            smer = toCanonical(smer);
    //        }

    //        const std::vector<int> kmers = cbf_k.querySmers(smer);
    //        const std::vector<int> smers = cbf_s.querySmers(smer);

    //        if (kmers.size() != 1) {
    //            std::cerr << "kmers.size() != 1" << std::endl;
    //        }
    //        if (smers.size() != 1) {
    //            std::cerr << "smers.size() != 1" << std::endl;
    //        }

    //        // std::cout << smer << " " << kmers[0] << " " << smers[0] << std::endl;
    //    } catch (const std::invalid_argument& e) {
    //        std::cerr << "Invalid argument when parsing line :\"" << line << "\"" << std::endl;
    //    }
    //}
    checkExists(filename_query);
    std::cout << "before matrix" << std::endl;
    cbf_s.increase_z_of(2);
    std::vector<std::vector<uint64_t>> mat_median = create_matrix(256);
    for (const auto& [read, header] : fimpera_lib::generators::ReadReader(filename_query)) {
        std::vector<int> rk = filter(cbf_k.queryRead(read));
        std::vector<int> rs = filter(cbf_s.queryRead(read));

        if ((rk.size() != 0) && (rs.size() != 0)) {
          if (rk.size() != rs.size()) {
              std::cerr << "rk.size() != rs.size()" << std::endl;
          }
          int mk = std::round(getAverage(rk));
          int ms = std::round(getAverage(rs));
          if (mk > ms) {
              std::cout << "mk > ms\n";
              printv(rk);
              printv(rs);
              printv(cbf_k.querySmers(read));
              printv(cbf_s.querySmers(read));
           }
            mat_median[mk][ms] += 1;
        }
    }
    std::cout << "matrix" << std::endl;
    print_matrix(mat_median);
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
