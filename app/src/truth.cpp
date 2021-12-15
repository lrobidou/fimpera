#include <math.h>  // TODO remove ?
#include <robin_hood.h>

#include <fimpera-lib/CBF.hpp>
#include <fimpera-lib/fimpera.hpp>

#include "args.hpp"

class ResultGetter : public CustomResponse {
   private:
    std::vector<int> entireResponse;

   public:
    ResultGetter() {}

    void
    processResult(const std::vector<int>& res, const unsigned int& K, const std::string& current_header, const std::string& current_read) {
        entireResponse.insert(std::end(entireResponse), std::begin(res), std::end(res));
    }

    std::vector<int> getResult() {
        return entireResponse;
    }
};

class TruthInTheShapeOfAnAMQ {
   private:
    uint64_t _nbBuckets;
    uint64_t _nbCells;
    uint64_t _limitValueInBucket;
    robin_hood::unordered_map<std::string, int> _t;

   public:
    TruthInTheShapeOfAnAMQ(int nbBits, int nbBuckets) : _nbBuckets(nbBuckets) {
        _nbCells = nbBits / _nbBuckets;
        _limitValueInBucket = pow(2, _nbBuckets) - 1;
    }

    bool set(const std::string& x, int occurrence = 1) {
        // get existing data
        int data = get(x);

        // compute new value of data
        if (data < occurrence) {
            data = occurrence;
            // TODO discuss with Pierre
            // if (data > _limitValueInBucket) {
            //     data = _limitValueInBucket;
            // }
        }

        auto it = _t.find(x);
        if (it != _t.end()) {
            it->second = data;
        } else {
            _t.insert({x, data});
        }
        return data;
    }

    int get(const std::string& x) const {
        auto it = _t.find(x);
        if (it != _t.end()) {
            return it->second;
        } else {
            return 0;
        }
    }
};

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

std::tuple<int, int, int, int> compareVectors(const std::vector<int>& result, const std::vector<int>& truth) {
    int tp = 0;
    int tn = 0;
    int fp = 0;
    int fn = 0;

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

void compareWithTruth(const std::string& indexFilename, const std::string& KMCFilename, const std::string& queryFile, uint64_t size, uint64_t nbBuckets) {
    // TODO move to an evaluation module
    // read / create indexes
    fimpera<countingBF::CBF> index = fimpera<countingBF::CBF>(indexFilename);
    fimpera<TruthInTheShapeOfAnAMQ> truth = fimpera<TruthInTheShapeOfAnAMQ>(KMCFilename, index.getK(), 0, index.getCanonical(), size, nbBuckets);
    fimpera<TruthInTheShapeOfAnAMQ> constrution_truth = fimpera<TruthInTheShapeOfAnAMQ>(KMCFilename, index.getK(), index.getz(), index.getCanonical(), size, nbBuckets);

    // create objects to get the result of the queries back (but one could chose to process each read independently instead)
    ResultGetter result_getter = ResultGetter();
    ResultGetter result_getter_truth = ResultGetter();
    ResultGetter result_getter_construction_truth = ResultGetter();

    // perform the queries
    index.query(queryFile, result_getter);
    truth.query(queryFile, result_getter_truth);
    constrution_truth.query(queryFile, result_getter_construction_truth);

    // exctract the results
    std::vector<int> index_response = result_getter.getResult();
    std::vector<int> truth_response = result_getter_truth.getResult();
    std::vector<int> construction_truth_response = result_getter_construction_truth.getResult();

    // compute the FPR and the construction FPR, then print them
    const auto& [tp, tn, fp, fn] = compareVectors(index_response, truth_response);
    std::cout << tp << " " << tn << " " << fp << " " << fn << std::endl;
    std::cout << "fpr = " << ((double)fp) / ((double)(fp + tn)) << std::endl;

    const auto& [tpc, tnc, fpc, fnc] = compareVectors(construction_truth_response, truth_response);
    std::cout << tpc << " " << tnc << " " << fpc << " " << fnc << std::endl;
    std::cout << "fprc = " << ((double)fpc) / ((double)(fpc + tnc)) << std::endl;

    const std::string name = std::to_string(index.getK()) + "_" + std::to_string(index.getz()) + "_" + std::to_string(nbBuckets);

    toFileTXT("histo_" + name + ".txt", getHistogram(index_response, truth_response));
    toFileTXT("histo_construction_" + name + ".txt", getHistogram(construction_truth_response, truth_response));
}

int main(int argc, char* argv[]) {
    argparse::ArgumentParser program("fimpera_index", "0.0.1");
    // mandatory arguments
    program.add_argument("input_filename").help("index you want to query");
    program.add_argument("query_filename").help("file you want to query against the index");
    program.add_argument("kmc_filename").help("kmc file that contains the truth for the Kmers");

    parse(program, argc, argv);
    // optional arguments
    // TODO use value stored in filter
    // program.add_argument("-K").help("size of Kmers").default_value(31).scan<'i', int>();
    // program.add_argument("-z").help("value of z (cf paper of findere)").default_value(3).scan<'i', int>();

    const std::string index_filename = program.get("input_filename");
    const std::string query_filename = program.get("query_filename");
    const std::string kmc_filename = program.get("kmc_filename");

    compareWithTruth(index_filename, kmc_filename, query_filename, 1, 1);  // TODO copy data from index ?
}