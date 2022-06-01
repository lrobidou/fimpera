#include <math.h>  // TODO remove ?
#include <robin_hood.h>

#include <fimpera-lib/CBF.hpp>
#include <fimpera-lib/ResultGetter.hpp>
#include <fimpera-lib/evaluation/UnlimitedTruthInTheShapeOfAnAMQ.hpp>
#include <fimpera-lib/fimpera.hpp>

#include "args.hpp"

inline void printv(const std::vector<int>& x) {
    for (auto y : x) {
        std::cout << y << " ";
    }
    std::cout << std::endl;
}

std::tuple<int, int, int, int> compareVectors(const std::vector<std::vector<int>>& truthv, const std::vector<std::vector<int>>& ctruthv, std::vector<std::vector<std::string>> kmers_reads_truth, std::vector<std::vector<std::string>> kmers_reads_ctruth) {
    std::string ctruth_tp = "ctruth_tp";
    std::string ctruth_tn = "ctruth_tn";
    std::string ctruth_fp = "ctruth_fp";
    std::string ctruth_fn = "ctruth_fn";

    remove(ctruth_tp.c_str());
    remove(ctruth_tn.c_str());
    remove(ctruth_fp.c_str());
    remove(ctruth_fn.c_str());

    // print keys and values to file
    std::ofstream ctruth_tp_file(ctruth_tp);
    std::ofstream ctruth_tn_file(ctruth_tn);
    std::ofstream ctruth_fp_file(ctruth_fp);
    std::ofstream ctruth_fn_file(ctruth_fn);

    if (truthv.size() != ctruthv.size()) {
        std::cerr << "truthv.size() != ctruthv.size()" << std::endl;
        exit(1);
    }

    int tp = 0;
    int tn = 0;
    int fp = 0;
    int fn = 0;

    for (std::vector<std::vector<int>>::size_type i = 0; i < ctruthv.size(); i++) {
        std::vector<int> truth = truthv[i];
        std::vector<int> ctruth = ctruthv[i];
        if (truth.size() != ctruth.size()) {
            std::cerr << "truth.size() != truth.size() for i = " << i << std::endl;
            exit(1);
        }

        for (std::size_t j = 0; j < truth.size(); j++) {
            if (kmers_reads_truth[i][j] != kmers_reads_ctruth[i][j]) {
                std::cerr << "kmers_reads_truth[i][j] != kmers_reads_ctruth[i][j]" << std::endl;
                exit(1);
            }
            std::string le_kmer = kmers_reads_truth[i][j];
            if (ctruth[j]) {
                if (truth[j]) {
                    ctruth_tp_file << le_kmer << "\n";
                    tp++;
                } else {
                    ctruth_fn_file << le_kmer << "\n";
                    fn++;
                }
            } else {
                if (truth[j]) {
                    ctruth_fp_file << le_kmer << "\n";
                    fp++;
                } else {
                    ctruth_tn_file << le_kmer << "\n";
                    tn++;
                }
            }
        }
    }
    return {tp, tn, fp, fn};
}

void compareWithTruth(const std::string& KMCFilename, const std::string& queryFile, int K, int z) {
    fimpera<UnlimitedTruthInTheShapeOfAnAMQ> truth = fimpera<UnlimitedTruthInTheShapeOfAnAMQ>(KMCFilename, K, 0, false, 0, 1000);
    fimpera<UnlimitedTruthInTheShapeOfAnAMQ> constrution_truth = fimpera<UnlimitedTruthInTheShapeOfAnAMQ>(KMCFilename, K, z, false, 0, 1000);
    std::cout << "yay" << std::endl;

    // create objects to get the result of the queries back (but one could chose to process each read independently instead)
    ResultGetter result_getter_truth = ResultGetter();
    ResultGetter result_getter_construction_truth = ResultGetter();

    // perform the queries
    truth.query(queryFile, result_getter_truth);
    constrution_truth.query(queryFile, result_getter_construction_truth);

    // exctract the results
    const auto& [truth_response, kmers_reads_truth] = result_getter_truth.getResult();
    const auto& [construction_truth_response, kmers_reads_ctruth] = result_getter_construction_truth.getResult();

    // compute the FPR and the construction FPR, then print them
    const auto& [tpc, tnc, fpc, fnc] = compareVectors(construction_truth_response, truth_response, kmers_reads_truth, kmers_reads_ctruth);
    std::cout << tpc << " " << tnc << " " << fpc << " " << fnc << std::endl;
    std::cout << "fprc = " << ((double)fpc) / ((double)(fpc + tnc)) << std::endl;

    // const std::string name = std::to_string(index.getK()) + "_" + std::to_string(index.getz()) + "_" + std::to_string(nbBuckets);

    // toFileTXT("histo_" + name + ".txt", getHistogram(index_response, truth_response));
    // toFileTXT("histo_construction_" + name + ".txt", getHistogram(construction_truth_response, truth_response));
}

int main(int argc, char* argv[]) {
    argparse::ArgumentParser program("fimpera_index", "0.0.1");
    // // mandatory arguments
    // program.add_argument("input_filename").help("index you want to query");
    program.add_argument("kmc_filename").help("kmc file that contains the truth for the Kmers");
    program.add_argument("query_filename").help("file you want to query against the index");
    program.add_argument("K").help("value of k").scan<'i', int>();
    program.add_argument("z").help("value of z").scan<'i', int>();

    parse(program, argc, argv);

    const std::string query_filename = program.get("query_filename");
    const std::string kmc_filename = program.get("kmc_filename");
    const int K = program.get<int>("K");
    const int z = program.get<int>("z");

    compareWithTruth(kmc_filename, query_filename, K, z);
}