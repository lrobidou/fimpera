#include <math.h>  // TODO remove ?
#include <robin_hood.h>

#include <chrono>
#include <fimpera-lib/CBF.hpp>
#include <fimpera-lib/evaluation/LimitedTruthInTheShapeOfAnAMQ.hpp>
#include <fimpera-lib/evaluation/UnlimitedTruthInTheShapeOfAnAMQ.hpp>
#include <fimpera-lib/fimpera.hpp>
#include <string>

#include "args.hpp"

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
    const int& b) {
    // std::size_t matrix_size = pow(2, b);
    std::size_t matrix_size = 20;  // TODO?

    std::vector<std::vector<uint64_t>> matrix_unlimited_truth = create_matrix(matrix_size);
    std::vector<std::vector<uint64_t>> matrix_limited_truth = create_matrix(matrix_size);
    std::vector<std::vector<uint64_t>> matrix_index = create_matrix(matrix_size);
    std::vector<std::vector<uint64_t>> matrix_unlimited_ctruth = create_matrix(matrix_size);
    std::vector<std::vector<uint64_t>> matrix_limited_ctruth = create_matrix(matrix_size);
    std::vector<std::vector<uint64_t>> matrix_index_z = create_matrix(matrix_size);

    FileManager reader = FileManager();
    reader.addFile(filename);
    std::string current_read;

    // uint64_t tp = 0;
    // uint64_t tn = 0;
    // uint64_t fp = 0;
    // uint64_t fn = 0;

    uint64_t nbIterMax = 100000;
    uint64_t i = 0;

    while ((!(current_read = reader.get_next_read()).empty()) && (i < nbIterMax)) {
        std::string current_data = reader.get_data();
        std::string current_header = current_data.substr(0, current_data.find('\n'));

        // std::vector<int> res = index.queryRead(current_read);
        // std::vector<int> res_truth = truth.queryRead(current_read);
        std::vector<int> res_unlimited_truth = unlimited_truth.queryRead(current_read);
        std::vector<int> res_limited_truth = limited_truth.queryRead(current_read);
        std::vector<int> res_index = index.queryRead(current_read);
        std::vector<int> res_unlimited_ctruth = unlimited_ctruth.queryRead(current_read);
        std::vector<int> res_limited_ctruth = limited_ctruth.queryRead(current_read);
        std::vector<int> res_index_z = index_z.queryRead(current_read);
        std::cout << "before push" << std::endl;
        // std::cout << res.size() << std::endl;
        for (std::size_t j = 0; j < res_unlimited_truth.size(); j++) {
            // if (j % 100000 == 0) {
            // std::cout << j << " " << res.size() << std::endl;
            // }
            matrix_unlimited_truth[res_unlimited_truth[j]][res_unlimited_ctruth[j]] += 1;
            matrix_limited_truth[res_limited_truth[j]][res_unlimited_ctruth[j]] += 1;
            matrix_index[res_index[j]][res_unlimited_ctruth[j]] += 1;
            matrix_unlimited_ctruth[res_unlimited_ctruth[j]][res_unlimited_ctruth[j]] += 1;
            matrix_limited_ctruth[res_limited_ctruth[j]][res_unlimited_ctruth[j]] += 1;
            matrix_index_z[res_index_z[j]][res_unlimited_ctruth[j]] += 1;
            // matrix[res_truth[j]][res[j]] += 1;
        }
        if (i % 100) {
            std::cout << "read " << i << "done" << std::endl;
        }

        // const auto& [tpp, tnp, fpp, fnp] = compareVectors(res, res_truth);
        // tp += tpp;
        // tn += tnp;
        // fp += fpp;
        // fn += fnp;

        // response.processResult(res, _k + _z, current_header, current_read);
        i++;
    }

    std::cout << "    matrix_unlimited_truth" << std::endl;
    print_matrix(matrix_unlimited_truth);
    std::cout << std::endl;
    std::cout << std::endl;

    std::cout << "matrix_limited_truth" << std::endl;
    print_matrix(matrix_limited_truth);
    std::cout << std::endl;
    std::cout << std::endl;

    std::cout << "matrix_index" << std::endl;
    print_matrix(matrix_index);
    std::cout << std::endl;
    std::cout << std::endl;

    std::cout << "matrix_unlimited_ctruth" << std::endl;
    print_matrix(matrix_unlimited_ctruth);
    std::cout << std::endl;
    std::cout << std::endl;

    std::cout << "matrix_limited_ctruth" << std::endl;
    print_matrix(matrix_limited_ctruth);
    std::cout << std::endl;
    std::cout << std::endl;

    std::cout << "matrix_index_z" << std::endl;
    print_matrix(matrix_index_z);
    std::cout << std::endl;
    std::cout << std::endl;

    // print(matrix);
}

inline std::string ReplaceAll(std::string str, const std::string& from, const std::string& to) {
    size_t start_pos = 0;
    while ((start_pos = str.find(from, start_pos)) != std::string::npos) {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length();  // Handles case where 'to' is a substring of 'from'
    }
    return str;
}

template <typename Strategy>
void compareWithTruth(const std::string& KMCFilename, const std::string& queryFile, uint64_t size, std::size_t K, int b, Strategy strategy) {
    const std::vector<int> zs = {5};
    auto start = std::chrono::steady_clock::now();

    fimpera<UnlimitedTruthInTheShapeOfAnAMQ> unlimited_truth = fimpera<UnlimitedTruthInTheShapeOfAnAMQ>(strategy, KMCFilename, K, 0, false, size, b);
    fimpera<LimitedTruthInTheShapeOfAnAMQ> limited_truth = fimpera<LimitedTruthInTheShapeOfAnAMQ>(strategy, KMCFilename, K, 0, false, size, b);
    fimpera<countingBF::CBF> index = fimpera<countingBF::CBF>(strategy, KMCFilename, K, 0, false, size, b);
    std::cout << "index truth in (ms)=" << since(start).count() << std::endl;

    for (int z : zs) {
        auto start_of_this_z = std::chrono::steady_clock::now();
        std::cout << "starting analyzing z = " << z << " after " << std::chrono::duration<double>(start_of_this_z - start).count() << " s." << std::endl;
        fimpera<UnlimitedTruthInTheShapeOfAnAMQ> unlimited_ctruth = fimpera<UnlimitedTruthInTheShapeOfAnAMQ>(strategy, KMCFilename, K, z, false, size, b);
        fimpera<LimitedTruthInTheShapeOfAnAMQ> limited_ctruth = fimpera<LimitedTruthInTheShapeOfAnAMQ>(strategy, KMCFilename, K, z, false, size, b);
        fimpera<countingBF::CBF> index_z = fimpera<countingBF::CBF>(strategy, KMCFilename, K, z, false, size, b);

        // std::string indexFilename = ReplaceAll(indexFilenameTemplate, "_z_", "_z" + std::to_string(z) + "_");
        // indexFilename = ReplaceAll(indexFilename, "_k_", "_k" + std::to_string(K) + "_");
        // std::cout << "index filename " << indexFilename << std::endl;

        // fimpera<countingBF::CBF> index = fimpera<countingBF::CBF>(indexFilename);
        // std::cout << "index BF in (ms)=" << since(start_of_this_z).count() << std::endl;

        queryLowMemory(unlimited_truth, limited_truth, index, unlimited_ctruth, limited_ctruth, index_z, queryFile, b);
    }
}

int main(int argc, char* argv[]) {
    argparse::ArgumentParser program("fimpera_index", "0.0.1");
    // mandatory arguments
    // program.add_argument("input_filename").help("index you want to query");
    program.add_argument("query_filename").help("file you want to query against the index");
    program.add_argument("kmc_filename").help("kmc file that contains the truth for the Kmers");
    program.add_argument("size").help("output index size").scan<'i', std::size_t>();

    program.add_argument("-K").help("size of Kmers").default_value(31).scan<'i', int>();
    // program.add_argument("-z").help("value of z (cf paper of findere)").default_value(3).scan<'i', int>();
    program.add_argument("b").help("the number of bits per buckets in the filter").scan<'i', int>();

    parse(program, argc, argv);

    // const std::string index_filename = program.get("input_filename");
    const std::string query_filename = program.get("query_filename");
    const std::string kmc_filename = program.get("kmc_filename");
    const std::size_t size = program.get<std::size_t>("size");
    const int b = program.get<int>("b");
    const int K = program.get<int>("-K");
    // const int z = program.get<int>("-z");
    // abundanceToIdentifierStrategy::identity idStrategy = abundanceToIdentifierStrategy::identity();
    abundanceToIdentifierStrategy::log2 logStrategy = abundanceToIdentifierStrategy::log2();

    compareWithTruth(kmc_filename, query_filename, size, K, b, logStrategy);
}
