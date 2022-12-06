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

inline robin_hood::unordered_map<int, int> getHistogram(std::vector<int> a, std::vector<int> b) {
    assert(a.size() == b.size());
    int n = a.size();
    robin_hood::unordered_map<int, int> histogram;
    for (int i = 0; i < n; ++i) {
        add(histogram, absDiff(a[i], b[i]), 1);
    }
    return histogram;
}

void inline toFileTXT(std::string outfilename, robin_hood::unordered_map<int, int> map) {
    remove(outfilename.c_str());

    // get sorted keys
    std::vector<int> keys;
    for (auto const& [key, b] : map) {
        keys.push_back(key);
    }
    sort(keys.begin(), keys.end());

    // print keys and values to file
    std::ofstream outFile(outfilename);
    for (const auto& key : keys) {
        outFile << key << " " << map[key] << "\n";
    }
}

void inline toFileTXT(std::string outfilename, std::vector<uint64_t> v) {
    remove(outfilename.c_str());

    // print keys and values to file
    std::ofstream outFile(outfilename);
    for (const auto& x : v) {
        outFile << x << ",";
    }
    outFile << "\n";
}

void inline toFileTXT(std::string outfilename, robin_hood::unordered_map<int, std::vector<int>> map) {
    remove(outfilename.c_str());

    // get sorted keys
    std::vector<int> keys;
    for (auto const& [key, b] : map) {
        keys.push_back(key);
    }
    sort(keys.begin(), keys.end());

    // print keys and values to file
    std::ofstream outFile(outfilename);
    for (const auto& key : keys) {
        outFile << key;
        std::vector<int> values = map[key];
        for (int value : values) {
            outFile << " " << value;
        }

        outFile << "\n";
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

void queryLowMemory(
    const fimpera<UnlimitedTruthInTheShapeOfAnAMQ>& unlimited_truth,
    const fimpera<LimitedTruthInTheShapeOfAnAMQ>& limited_truth,
    const fimpera<countingBF::CBF>& index,
    const fimpera<UnlimitedTruthInTheShapeOfAnAMQ>& unlimited_ctruth,
    const fimpera<LimitedTruthInTheShapeOfAnAMQ>& limited_ctruth,
    const fimpera<countingBF::CBF>& index_z,
    const std::string& filename,
    const int& b,
    bool wrongKmersToAFile) {
    // std::size_t matrix_size = pow(2, b);
    std::size_t matrix_size = 20;  // TODO?

    std::vector<std::vector<uint64_t>> matrix_unlimited_truth_vs_truth = create_matrix(matrix_size);
    std::vector<std::vector<uint64_t>> matrix_ctruth_vs_truth = create_matrix(matrix_size);
    std::vector<std::vector<uint64_t>> matrix_index_z_vs_truth = create_matrix(matrix_size);
    std::vector<std::vector<uint64_t>> matrix_index_vs_index_z = create_matrix(matrix_size);
    std::vector<std::vector<uint64_t>> matrix_index_vs_truth = create_matrix(matrix_size);
    // std::vector<std::vector<uint64_t>> matrix_limited_ctruth = create_matrix(matrix_size);
    // std::vector<std::vector<uint64_t>> matrix_index_z = create_matrix(matrix_size);

    std::string matrix_unlimited_truth_vs_truth_filename = "matrix_unlimited_truth_vs_truth";
    std::string matrix_ctruth_vs_truth_filename = "matrix_ctruth_vs_truth";
    std::string matrix_index_z_vs_truth_filename = "matrix_index_z_vs_truth";
    std::string matrix_index_vs_index_z_filename = "matrix_index_vs_index_z";
    std::string matrix_matrix_index_vs_truth_filename = "matrix_index_vs_truth";

    // std::string matrix_limited_ctruth_filename = "matrix_limited_ctruth";
    // std::string matrix_index_z_filename = "matrix_index_z";

    remove(matrix_unlimited_truth_vs_truth_filename.c_str());
    remove(matrix_ctruth_vs_truth_filename.c_str());
    remove(matrix_index_z_vs_truth_filename.c_str());
    remove(matrix_index_vs_index_z_filename.c_str());
    remove(matrix_matrix_index_vs_truth_filename.c_str());
    // remove(matrix_limited_ctruth_filename.c_str());
    // remove(matrix_index_z_filename.c_str());

    std::ofstream matrix_unlimited_truth_vs_truth_file(matrix_unlimited_truth_vs_truth_filename);
    std::ofstream matrix_ctruth_vs_truth_file(matrix_ctruth_vs_truth_filename);
    std::ofstream matrix_index_z_vs_truth_file(matrix_index_z_vs_truth_filename);
    std::ofstream matrix_index_vs_index_z_file(matrix_index_vs_index_z_filename);
    std::ofstream matrix_index_vs_truth_file(matrix_matrix_index_vs_truth_filename);
    // std::ofstream matrix_limited_ctruth_file(matrix_limited_ctruth_filename);
    // std::ofstream matrix_index_z_file(matrix_index_z_filename);

    FileManager reader = FileManager();
    reader.addFile(filename);
    std::string current_read;

    uint64_t nbIterMax = 100000;
    uint64_t i = 0;

    while ((!(current_read = reader.get_next_read()).empty()) && (i < nbIterMax)) {
        std::string current_data = reader.get_data();
        std::string current_header = current_data.substr(0, current_data.find('\n'));

        std::vector<int> res_unlimited_truth = unlimited_truth.queryRead(current_read);
        std::vector<int> res_truth = limited_truth.queryRead(current_read);
        std::vector<int> res_index = index.queryRead(current_read);
        // std::vector<int> res_unlimited_ctruth = unlimited_ctruth.queryRead(current_read);
        std::vector<int> res_ctruth = limited_ctruth.queryRead(current_read);
        std::vector<int> res_index_z = index_z.queryRead(current_read);

        int k = limited_truth.get_k();

        std::vector<std::string> kmers = std::vector<std::string>();
        kmers.reserve(current_read.size() - k + 1);

        for (std::size_t start = 0; start < current_read.size() - k + 1; start++) {
            kmers.push_back(current_read.substr(start, k));
        }

        for (std::size_t j = 0; j < res_unlimited_truth.size(); j++) {
            if (res_truth[j] != -1) {
                matrix_unlimited_truth_vs_truth[res_truth[j]][res_unlimited_truth[j]] += 1;
                matrix_ctruth_vs_truth[res_truth[j]][res_ctruth[j]] += 1;
                matrix_index_z_vs_truth[res_truth[j]][res_index_z[j]] += 1;
                matrix_index_vs_index_z[res_index_z[j]][res_index[j]] += 1;
                matrix_index_vs_truth[res_truth[j]][res_index[j]] += 1;
                // matrix_limited_ctruth[res_limited_ctruth[j]][res_unlimited_ctruth[j]] += 1;
                // matrix_index_z[res_index_z[j]][res_unlimited_ctruth[j]] += 1;

                if (wrongKmersToAFile) {
                    if (res_unlimited_truth[j] > res_truth[j]) {
                        matrix_unlimited_truth_vs_truth_file << kmers[j] << " " << res_unlimited_truth[j] - res_truth[j] << "\n";
                    }
                    if (res_ctruth[j] > res_truth[j]) {
                        matrix_ctruth_vs_truth_file << kmers[j] << " " << res_ctruth[j] - res_truth[j] << "\n";
                    }
                    if (res_index_z[j] > res_truth[j]) {
                        matrix_index_z_vs_truth_file << kmers[j] << " " << res_index_z[j] - res_truth[j] << "\n";
                    }
                    if (res_index[j] > res_index_z[j]) {
                        matrix_index_vs_index_z_file << kmers[j] << " " << res_index[j] - res_index_z[j] << "\n";
                    }
                    if (res_index[j] > res_truth[j]) {
                        matrix_index_vs_truth_file << kmers[j] << " " << res_index[j] - res_truth[j] << "\n";
                    }
                    // if (res_unlimited_ctruth[j] > res_limited_ctruth[j]) {
                    //     matrix_limited_ctruth_file << kmers[j] << " " << res_unlimited_ctruth[j] - res_limited_ctruth[j] << "\n";
                    // }
                    // if (res_unlimited_ctruth[j] > res_index_z[j]) {
                    //     matrix_index_z_file << kmers[j] << " " << res_unlimited_ctruth[j] - res_index_z[j] << "\n";
                }
            }
        }
        i++;
    }

    std::cout << "matrix_unlimited_truth_vs_truth" << std::endl;
    print_matrix(matrix_unlimited_truth_vs_truth);
    std::cout << std::endl;
    std::cout << std::endl;

    std::cout << "matrix_ctruth_vs_truth" << std::endl;
    print_matrix(matrix_ctruth_vs_truth);
    std::cout << std::endl;
    std::cout << std::endl;

    std::cout << "matrix_index_z_vs_truth" << std::endl;
    print_matrix(matrix_index_z_vs_truth);
    std::cout << std::endl;
    std::cout << std::endl;

    std::cout << "matrix_index_vs_index_z" << std::endl;
    print_matrix(matrix_index_vs_index_z);
    std::cout << std::endl;
    std::cout << std::endl;

    std::cout << "matrix_index_vs_truth" << std::endl;
    print_matrix(matrix_index_vs_truth);
    std::cout << std::endl;
    std::cout << std::endl;

    // std::cout << "matrix_limited_ctruth" << std::endl;
    // print_matrix(matrix_limited_ctruth);
    // std::cout << std::endl;
    // std::cout << std::endl;

    // std::cout << "matrix_index_z" << std::endl;
    // print_matrix(matrix_index_z);
    // std::cout << std::endl;
    // std::cout << std::endl;

    // print(matrix);
}

template <typename Strategy>
void compareSmersAndKmers_in_UnlimitedTruthInTheShapeOfAnAMQ(const std::string& kmc_filename_kmers, const std::string& kmc_filename_smers, std::size_t size, int b, Strategy strategy) {
    bool _canonical = true;
    auto start = std::chrono::steady_clock::now();

    fimpera<UnlimitedTruthInTheShapeOfAnAMQ> unlimited_truth_k = fimpera<UnlimitedTruthInTheShapeOfAnAMQ>(strategy, kmc_filename_kmers, 31, 3, false, size, b);  // on stocke des 28-mers

    std::cout << "31" << std::endl;

    fimpera<UnlimitedTruthInTheShapeOfAnAMQ> unlimited_truth_s = fimpera<UnlimitedTruthInTheShapeOfAnAMQ>(strategy, kmc_filename_smers, 28, 0, false, size, b);  // on stocke des 28-mers
    std::cout << "28" << std::endl;

    checkExists(kmc_filename_smers);

    std::ifstream myFileGz(kmc_filename_smers);
    zstr::istream myFile(myFileGz);

    std::string line;
    while (std::getline(myFile, line)) {  // \n are removed
        std::string smer;
        std::string abundanceStr;
        std::stringstream linestream(line);
        std::getline(linestream, smer, '\t');
        std::getline(linestream, abundanceStr, '\t');

        if (smer.size() != 28) {
            std::cerr << "smer.size() != 28" << std::endl;
        }

        try {
            uint64_t abundance = std::stoll(abundanceStr);
            uint64_t id = strategy.fct(abundance);
            if (_canonical) {
                smer = toCanonical(smer);
            }

            const std::vector<int> kmers = unlimited_truth_k.querySmers(smer);
            const std::vector<int> smers = unlimited_truth_s.querySmers(smer);

            if (kmers.size() != 1) {
                std::cerr << "kmers.size() != 1" << std::endl;
            }
            if (smers.size() != 1) {
                std::cerr << "smers.size() != 1" << std::endl;
            }

            std::cout << kmers[0] << " " << smers[0] << std::endl;
        } catch (const std::invalid_argument& e) {
            std::cerr << "Invalid argument when parsing line :\"" << line << "\"" << std::endl;
        }
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

template <typename Strategy>
void compareSmersAndKmers_in_cBF(const std::string& kmc_filename_kmers, const std::string& kmc_filename_smers, std::size_t size, int b, Strategy strategy, const std::string& filename_query) {
    bool _canonical = true;
    auto start = std::chrono::steady_clock::now();

    fimpera<countingBF::CBF> cbf_k = fimpera<countingBF::CBF>(strategy, kmc_filename_kmers, 31, 2, true, size, b);  // on stocke des 29-mers

    fimpera<countingBF::CBF> cbf_s = fimpera<countingBF::CBF>(strategy, kmc_filename_smers, 29, 0, true, size, b);  // on stocke des 29-mers

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
        
        std::vector<int> rk_no_filter = cbf_k.queryRead(read);
        std::vector<int> rs_no_filter = cbf_s.queryRead(read);
        std::vector<int> rk = filter(rk_no_filter);
        std::vector<int> rs = filter(rs_no_filter);

        if ((rk.size() != 0) && (rs.size() != 0)) {
          if (rk.size() != rs.size()) {std::cerr << "rk.size() != rs.size()" << std::endl;}
            int mk = median(rk.begin(), rk.end());
            int ms = median(rs.begin(), rs.end());
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
