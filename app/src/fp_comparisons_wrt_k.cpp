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

// kmc -v -fq -k29 -ci2 /groups/genscale/NGSdatasets/metagenomics/TARA/tara_set3/QQSS/76SUR1QQSS11/AHX_AMQIOSF_1_2_C7RY5ACXX.IND12_clean.fastq.gz tmp.res KMC_tmp
// kmc_tools dump tmp.res AHX_AMQIOSF_ci2_k29_canonical.txt
// rm tmp.res.kmc_*

/**
 * Produces TODO
 * For a fixed s, how does z influence the resqult quality of the query ?
 * At fixed s, k will increase.
 */

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
    const fimpera<LimitedTruthInTheShapeOfAnAMQ>& limited_truth,
    const fimpera<LimitedTruthInTheShapeOfAnAMQ>& limited_construction_truth,
    const fimpera<countingBF::CBF>& index,
    const std::string& filename,
    const int& b) {
    // std::size_t matrix_size = pow(2, b);
    std::size_t matrix_size = 50;  // TODO?

    std::vector<std::vector<uint64_t>> matrix_ctruth_vs_truth = create_matrix(matrix_size);
    std::vector<std::vector<uint64_t>> matrix_index_z_vs_truth = create_matrix(matrix_size);

    // std::string matrix_index_vs_truth_filename = "matrix_index_vs_truth_z_is_" + std::to_string(z);
    // std::string matrix_limited_ctruth_filename = "matrix_limited_ctruth";
    // std::string matrix_index_z_filename = "matrix_index_z";

    // remove(matrix_unlimited_truth_vs_truth_filename.c_str());
    // remove(matrix_ctruth_vs_truth_filename.c_str());
    // remove(matrix_index_z_vs_truth_filename.c_str());
    // remove(matrix_index_vs_index_z_filename.c_str());
    // remove(matrix_matrix_index_vs_truth_filename.c_str());
    // remove(matrix_limited_ctruth_filename.c_str());
    // remove(matrix_index_z_filename.c_str());

    // std::ofstream matrix_unlimited_truth_vs_truth_file(matrix_unlimited_truth_vs_truth_filename);
    // std::ofstream matrix_ctruth_vs_truth_file(matrix_ctruth_vs_truth_filename);
    // std::ofstream matrix_index_z_vs_truth_file(matrix_index_z_vs_truth_filename);
    // std::ofstream matrix_index_vs_index_z_file(matrix_index_vs_index_z_filename);
    // std::ofstream matrix_index_vs_truth_file(matrix_matrix_index_vs_truth_filename);
    // std::ofstream matrix_limited_ctruth_file(matrix_limited_ctruth_filename);
    // std::ofstream matrix_index_z_file(matrix_index_z_filename);

    FileManager reader = FileManager();
    reader.addFile(filename);
    std::string current_read;

    // uint64_t nbIterMax = 100000;  // 100000;
    uint64_t i = 0;

    int k = limited_truth.get_k();
    std::cout << "start query, k = " << k << std::endl;

    while ((!(current_read = reader.get_next_read()).empty())) {  // && (i < nbIterMax)) {
        std::string current_data = reader.get_data();
        std::string current_header = current_data.substr(0, current_data.find('\n'));
        // std::cout << "A" << std::endl;
        std::vector<int> res_truth = limited_truth.queryRead(current_read);
        // std::cout << "B" << std::endl;
        std::vector<int> res_construction_truth = limited_construction_truth.queryRead(current_read);
        // std::cout << "C" << std::endl;
        std::vector<int> res_index = index.queryRead(current_read);
        // std::cout << "D" << std::endl;
        // std::vector<int> res_unlimited_ctruth = unlimited_ctruth.queryRead(current_read);
        // std::vector<int> res_ctruth = limited_ctruth.queryRead(current_read);
        // std::vector<int> res_index_z = index_z.queryRead(current_read);

        // std::vector<std::string> kmers = std::vector<std::string>();
        // std::cout << "current_read.size() = " << current_read.size() << std::endl;
        // kmers.reserve(current_read.size() - k + 1);

        // for (std::size_t start = 0; start < current_read.size() - k + 1; start++) {
        //     kmers.push_back(current_read.substr(start, k));
        // }

        // std::cout << "E" << std::endl;

        for (std::size_t j = 0; j < res_truth.size(); j++) {
            if (res_truth[j] != -1) {
                matrix_ctruth_vs_truth[res_truth[j]][res_construction_truth[j]] += 1;
                matrix_index_z_vs_truth[res_truth[j]][res_index[j]] += 1;
                if (res_construction_truth[j] < res_truth[j]) {
                    std::cout << "kmer " << current_read.substr(j, k) << " is found " << res_construction_truth[j] << " by construction, but is found " << res_truth[j] << " times in the ground truth\n";
                }
                // matrix_limited_ctruth[res_limited_ctruth[j]][res_unlimited_ctruth[j]] += 1;
                // matrix_index_z[res_index_z[j]][res_unlimited_ctruth[j]] += 1;
            }
        }
        i++;
    }

    // std::cout << "matrix_unlimited_truth_vs_truth" << std::endl;
    // print_matrix(matrix_unlimited_truth_vs_truth);
    // std::cout << std::endl;
    // std::cout << std::endl;

    std::cout << "matrix_ctruth_vs_truth" << std::endl;
    print_matrix(matrix_ctruth_vs_truth);
    std::cout << std::endl;
    std::cout << std::endl;

    std::cout << "matrix_index_z_vs_truth" << std::endl;
    print_matrix(matrix_index_z_vs_truth);
    std::cout << std::endl;
    std::cout << std::endl;

    // std::cout << "matrix_index_vs_index_z" << std::endl;
    // print_matrix(matrix_index_vs_index_z);
    // std::cout << std::endl;
    // std::cout << std::endl;

    // std::cout << "matrix_index_vs_truth" << std::endl;
    // print_matrix(matrix_index_vs_truth);
    // std::cout << std::endl;
    // std::cout << std::endl;

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
void getSpeed(
    const std::string& kmc_filename_25_mers,
    const std::string& kmc_filename_26_mers,
    const std::string& kmc_filename_27_mers,
    const std::string& kmc_filename_28_mers,
    const std::string& kmc_filename_29_mers,
    const std::string& kmc_filename_30_mers,
    const std::string& kmc_filename_31_mers,
    const std::string& kmc_filename_32_mers,
    const std::string& kmc_filename_33_mers,
    const std::string& kmc_filename_60_mers,
    //   const std::string& kmc_filename_34_mers,
    //   const std::string& kmc_filename_35_mers,
    //   const std::string& kmc_filename_37_mers,
    //   const std::string& kmc_filename_48_mers,
    std::size_t size_cbf,
    const int k,
    const int b,
    Strategy strategy,
    const std::string& query_filename) {
    checkExists(kmc_filename_25_mers);
    checkExists(kmc_filename_26_mers);
    checkExists(kmc_filename_27_mers);
    checkExists(kmc_filename_28_mers);
    checkExists(kmc_filename_29_mers);
    checkExists(kmc_filename_30_mers);
    checkExists(kmc_filename_31_mers);
    checkExists(kmc_filename_32_mers);
    checkExists(kmc_filename_33_mers);

    checkExists(query_filename);

    std::cout << "every file exixts" << std::endl;

    fimpera<countingBF::CBF> fimpera_25_mers(strategy, kmc_filename_25_mers, k, 0, true, size_cbf, b, 1);
    // fimpera<countingBF::CBF> fimpera_25_mers(strategy, kmc_filename_28_mers, k, 0, true, size_cbf, b, 1);

    std::cout << "index done" << std::endl;

    // it does not make any sens to give a size to
    // fimpera<LimitedTruthInTheShapeOfAnAMQ>, but it is required to give one
    // to comply the same interface as fimpera<countingBF::CBF>
    // let's just use a random number
    uint64_t nbBitTruth = 100;

    int initial_k = fimpera_25_mers.get_k();
    fimpera<LimitedTruthInTheShapeOfAnAMQ> construction_truth_s = fimpera<LimitedTruthInTheShapeOfAnAMQ>(strategy, kmc_filename_25_mers, k, 0, true, nbBitTruth, b, 1);
    std::cout << "construction truth (k=" << initial_k << ") initialised" << std::endl;

    {
        std::cout << ">>> z = " << fimpera_25_mers.get_z() << std::endl;
        std::cout << ">>> k = " << fimpera_25_mers.get_k() << std::endl;
        int k = fimpera_25_mers.get_k();
        fimpera<LimitedTruthInTheShapeOfAnAMQ> truth_k = fimpera<LimitedTruthInTheShapeOfAnAMQ>(strategy, kmc_filename_25_mers, k, 0, true, nbBitTruth, b, 1);
        std::cout << "truth (k=" << k << ") initialised" << std::endl;
        queryLowMemory(truth_k, construction_truth_s, fimpera_25_mers, query_filename, b);
    }

    fimpera_25_mers.increase_z_of(1);
    construction_truth_s.increase_z_of(1);

    {
        std::cout << ">>> z = " << fimpera_25_mers.get_z() << std::endl;
        std::cout << ">>> k = " << fimpera_25_mers.get_k() << std::endl;
        int k = fimpera_25_mers.get_k();
        fimpera<LimitedTruthInTheShapeOfAnAMQ> truth_k = fimpera<LimitedTruthInTheShapeOfAnAMQ>(strategy, kmc_filename_26_mers, k, 0, true, nbBitTruth, b, 1);
        std::cout << "truth (k=" << k << ") initialised" << std::endl;
        queryLowMemory(truth_k, construction_truth_s, fimpera_25_mers, query_filename, b);
    }

    fimpera_25_mers.increase_z_of(1);
    construction_truth_s.increase_z_of(1);

    {
        std::cout << ">>> z = " << fimpera_25_mers.get_z() << std::endl;
        std::cout << ">>> k = " << fimpera_25_mers.get_k() << std::endl;
        int k = fimpera_25_mers.get_k();
        fimpera<LimitedTruthInTheShapeOfAnAMQ> truth_k = fimpera<LimitedTruthInTheShapeOfAnAMQ>(strategy, kmc_filename_27_mers, k, 0, true, nbBitTruth, b, 1);
        std::cout << "truth (k=" << k << ") initialised" << std::endl;
        queryLowMemory(truth_k, construction_truth_s, fimpera_25_mers, query_filename, b);
    }

    fimpera_25_mers.increase_z_of(1);
    construction_truth_s.increase_z_of(1);

    {
        std::cout << ">>> z = " << fimpera_25_mers.get_z() << std::endl;
        std::cout << ">>> k = " << fimpera_25_mers.get_k() << std::endl;
        int k = fimpera_25_mers.get_k();
        fimpera<LimitedTruthInTheShapeOfAnAMQ> truth_k = fimpera<LimitedTruthInTheShapeOfAnAMQ>(strategy, kmc_filename_28_mers, k, 0, true, nbBitTruth, b, 1);
        std::cout << "truth (k=" << k << ") initialised" << std::endl;
        queryLowMemory(truth_k, construction_truth_s, fimpera_25_mers, query_filename, b);
    }

    fimpera_25_mers.increase_z_of(1);
    construction_truth_s.increase_z_of(1);

    {
        std::cout << ">>> z = " << fimpera_25_mers.get_z() << std::endl;
        std::cout << ">>> k = " << fimpera_25_mers.get_k() << std::endl;
        int k = fimpera_25_mers.get_k();
        fimpera<LimitedTruthInTheShapeOfAnAMQ> truth_k = fimpera<LimitedTruthInTheShapeOfAnAMQ>(strategy, kmc_filename_29_mers, k, 0, true, nbBitTruth, b, 1);
        std::cout << "truth (k=" << k << ") initialised" << std::endl;
        queryLowMemory(truth_k, construction_truth_s, fimpera_25_mers, query_filename, b);
    }

    fimpera_25_mers.increase_z_of(1);
    construction_truth_s.increase_z_of(1);

    {
        std::cout << ">>> z = " << fimpera_25_mers.get_z() << std::endl;
        std::cout << ">>> k = " << fimpera_25_mers.get_k() << std::endl;
        int k = fimpera_25_mers.get_k();
        fimpera<LimitedTruthInTheShapeOfAnAMQ> truth_k = fimpera<LimitedTruthInTheShapeOfAnAMQ>(strategy, kmc_filename_30_mers, k, 0, true, nbBitTruth, b, 1);
        std::cout << "truth (k=" << k << ") initialised" << std::endl;
        queryLowMemory(truth_k, construction_truth_s, fimpera_25_mers, query_filename, b);
    }

    fimpera_25_mers.increase_z_of(1);
    construction_truth_s.increase_z_of(1);

    {
        std::cout << ">>> z = " << fimpera_25_mers.get_z() << std::endl;
        std::cout << ">>> k = " << fimpera_25_mers.get_k() << std::endl;
        int k = fimpera_25_mers.get_k();
        fimpera<LimitedTruthInTheShapeOfAnAMQ> truth_k = fimpera<LimitedTruthInTheShapeOfAnAMQ>(strategy, kmc_filename_31_mers, k, 0, true, nbBitTruth, b, 1);
        std::cout << "truth (k=" << k << ") initialised" << std::endl;
        queryLowMemory(truth_k, construction_truth_s, fimpera_25_mers, query_filename, b);
    }

    fimpera_25_mers.increase_z_of(1);
    construction_truth_s.increase_z_of(1);

    {
        std::cout << ">>> z = " << fimpera_25_mers.get_z() << std::endl;
        std::cout << ">>> k = " << fimpera_25_mers.get_k() << std::endl;
        int k = fimpera_25_mers.get_k();
        fimpera<LimitedTruthInTheShapeOfAnAMQ> truth_k = fimpera<LimitedTruthInTheShapeOfAnAMQ>(strategy, kmc_filename_32_mers, k, 0, true, nbBitTruth, b, 1);
        std::cout << "truth (k=" << k << ") initialised" << std::endl;
        queryLowMemory(truth_k, construction_truth_s, fimpera_25_mers, query_filename, b);
    }

    fimpera_25_mers.increase_z_of(1);
    construction_truth_s.increase_z_of(1);

    {
        std::cout << ">>> z = " << fimpera_25_mers.get_z() << std::endl;
        std::cout << ">>> k = " << fimpera_25_mers.get_k() << std::endl;
        int k = fimpera_25_mers.get_k();
        fimpera<LimitedTruthInTheShapeOfAnAMQ> truth_k = fimpera<LimitedTruthInTheShapeOfAnAMQ>(strategy, kmc_filename_33_mers, k, 0, true, nbBitTruth, b, 1);
        std::cout << "truth (k=" << k << ") initialised" << std::endl;
        queryLowMemory(truth_k, construction_truth_s, fimpera_25_mers, query_filename, b);
    }

    fimpera_25_mers.increase_z_of(27);
    construction_truth_s.increase_z_of(27);

    {
        std::cout << ">>> z = " << fimpera_25_mers.get_z() << std::endl;
        std::cout << ">>> k = " << fimpera_25_mers.get_k() << std::endl;
        int k = fimpera_25_mers.get_k();
        fimpera<LimitedTruthInTheShapeOfAnAMQ> truth_k = fimpera<LimitedTruthInTheShapeOfAnAMQ>(strategy, kmc_filename_60_mers, k, 0, true, nbBitTruth, b, 1);
        std::cout << "truth (k=" << k << ") initialised" << std::endl;
        queryLowMemory(truth_k, construction_truth_s, fimpera_25_mers, query_filename, b);
    }

    // fimpera_25_mers.increase_z_of(1);
    // construction_truth_s.increase_z_of(1);

    // {
    //     std::cout << ">>> z = " << fimpera_25_mers.get_z() << std::endl;
    //     std::cout << ">>> k = " << fimpera_25_mers.get_k() << std::endl;
    //     int k = fimpera_25_mers.get_k();
    //     fimpera<LimitedTruthInTheShapeOfAnAMQ> truth_k = fimpera<LimitedTruthInTheShapeOfAnAMQ>(strategy, kmc_filename_34_mers, k, 0, true, nbBitTruth, b, 1);
    //     std::cout << "truth (k=" << k << ") initialised" << std::endl;
    //     queryLowMemory(truth_k, construction_truth_s, fimpera_25_mers, query_filename, b);
    // }

    // fimpera_25_mers.increase_z_of(1);
    // construction_truth_s.increase_z_of(1);

    // {
    //     std::cout << ">>> z = " << fimpera_25_mers.get_z() << std::endl;
    //     std::cout << ">>> k = " << fimpera_25_mers.get_k() << std::endl;
    //     int k = fimpera_25_mers.get_k();
    //     fimpera<LimitedTruthInTheShapeOfAnAMQ> truth_k = fimpera<LimitedTruthInTheShapeOfAnAMQ>(strategy, kmc_filename_35_mers, k, 0, true, nbBitTruth, b, 1);
    //     std::cout << "truth (k=" << k << ") initialised" << std::endl;
    //     queryLowMemory(truth_k, construction_truth_s, fimpera_25_mers, query_filename, b);
    // }

    // fimpera_25_mers.increase_z_of(2);
    // construction_truth_s.increase_z_of(2);

    // {
    //     std::cout << ">>> z = " << fimpera_25_mers.get_z() << std::endl;
    //     int k = fimpera_25_mers.get_k();
    //     fimpera<LimitedTruthInTheShapeOfAnAMQ> truth_k = fimpera<LimitedTruthInTheShapeOfAnAMQ>(strategy, kmc_filename_37_mers, k, 0, true, nbBitTruth, b, 1);
    //     std::cout << "truth (k=" << k << ") initialised" << std::endl;
    //     queryLowMemory(truth_k, construction_truth_s, fimpera_25_mers, query_filename, b);
    // }

    // fimpera_25_mers.increase_z_of(11);
    // construction_truth_s.increase_z_of(11);

    // {
    //     std::cout << ">>> z = " << fimpera_25_mers.get_z() << std::endl;
    //     int k = fimpera_25_mers.get_k();
    //     fimpera<LimitedTruthInTheShapeOfAnAMQ> truth_k = fimpera<LimitedTruthInTheShapeOfAnAMQ>(strategy, kmc_filename_48_mers, k, 0, true, nbBitTruth, b, 1);
    //     std::cout << "truth (k=" << k << ") initialised" << std::endl;
    //     queryLowMemory(truth_k, construction_truth_s, fimpera_25_mers, query_filename, b);
    // }
}

int main(int argc, char* argv[]) {
    argparse::ArgumentParser program("fimpera_index", "0.0.1");
    // mandatory arguments
    program.add_argument("kmc_filename_25-mers").help("25-mer file you want to index");
    program.add_argument("kmc_filename_26-mers").help("26-mer file you want to index");
    program.add_argument("kmc_filename_27-mers").help("27-mer file you want to index");
    program.add_argument("kmc_filename_28-mers").help("28-mer file you want to index");
    program.add_argument("kmc_filename_29-mers").help("29-mer file you want to index");
    program.add_argument("kmc_filename_30-mers").help("30-mer file you want to index");
    program.add_argument("kmc_filename_31-mers").help("31-mer file you want to index");
    program.add_argument("kmc_filename_32-mers").help("32-mer file you want to index");
    program.add_argument("kmc_filename_33-mers").help("33-mer file you want to index");
    program.add_argument("kmc_filename_60-mers").help("60-mer file you want to index");
    // program.add_argument("kmc_filename_34-mers").help("34-mer file you want to index");
    // program.add_argument("kmc_filename_35-mers").help("35-mer file you want to index");
    // program.add_argument("kmc_filename_37-mers").help("37-mer file you want to index");
    // program.add_argument("kmc_filename_48-mers").help("48-mer file you want to index");

    program.add_argument("filename_query").help("file you want to query against the index");
    program.add_argument("size_cbf").help("output index size").scan<'i', std::size_t>();
    program.add_argument("k").help("k").scan<'i', int>();
    program.add_argument("b").help("the number of bits per buckets in the filter").scan<'i', int>();

    parse(program, argc, argv);
    const std::string kmc_filename_25_mers = program.get("kmc_filename_25-mers");
    std::cout << "ok" << std::endl;
    const std::string kmc_filename_26_mers = program.get("kmc_filename_26-mers");
    std::cout << "ok" << std::endl;
    const std::string kmc_filename_27_mers = program.get("kmc_filename_27-mers");
    std::cout << "ok" << std::endl;
    const std::string kmc_filename_28_mers = program.get("kmc_filename_28-mers");
    const std::string kmc_filename_29_mers = program.get("kmc_filename_29-mers");
    const std::string kmc_filename_30_mers = program.get("kmc_filename_30-mers");
    const std::string kmc_filename_31_mers = program.get("kmc_filename_31-mers");
    const std::string kmc_filename_32_mers = program.get("kmc_filename_32-mers");
    const std::string kmc_filename_33_mers = program.get("kmc_filename_33-mers");
    std::cout << "ok" << std::endl;
    const std::string kmc_filename_60_mers = program.get("kmc_filename_60-mers");

    // const std::string kmc_filename_34_mers = program.get("kmc_filename_34-mers");
    // const std::string kmc_filename_35_mers = program.get("kmc_filename_35-mers");
    // const std::string kmc_filename_37_mers = program.get("kmc_filename_37-mers");
    // const std::string kmc_filename_48_mers = program.get("kmc_filename_48-mers");

    // 3480000000.0
    const std::string query_filename = program.get("filename_query");

    const std::size_t size_cbf = program.get<std::size_t>("size_cbf");
    const int k = program.get<int>("k");
    const int b = program.get<int>("b");

    abundanceToIdentifierStrategy::log2 logStrategy = abundanceToIdentifierStrategy::log2();
    // getSpeed(kmc_filename_28_mers,
    //          kmc_filename_29_mers,
    //          kmc_filename_30_mers,
    //          kmc_filename_31_mers,
    //          kmc_filename_32_mers,
    //          kmc_filename_33_mers,
    //          kmc_filename_34_mers,
    //          kmc_filename_35_mers,
    //          kmc_filename_37_mers,
    //          kmc_filename_48_mers,
    //          size_cbf,
    //          k,
    //          b,
    //          logStrategy,
    //          query_filename);
    getSpeed(
        kmc_filename_25_mers,
        kmc_filename_26_mers,
        kmc_filename_27_mers,
        kmc_filename_28_mers,
        kmc_filename_29_mers,
        kmc_filename_30_mers,
        kmc_filename_31_mers,
        kmc_filename_32_mers,
        kmc_filename_33_mers,
        //  kmc_filename_34_mers,
        //  kmc_filename_35_mers,
        //  kmc_filename_37_mers,
        //  kmc_filename_48_mers,
        kmc_filename_60_mers,
        size_cbf,
        k,
        b,
        logStrategy,
        query_filename);
}
