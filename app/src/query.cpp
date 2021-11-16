#include <math.h>  // TODO remove ?
#include <robin_hood.h>

#include <fimpera-lib/CBF.hpp>
#include <fimpera-lib/fimpera.hpp>

#include "args/query_args.hpp"

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

    // print kays and values to file
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

    for (int i = 0; i < result.size(); i++) {
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

void compareWithTruth(const std::string& indexFilename, const std::string& KMCFilename, const std::string& queryFile, uint64_t size, uint64_t nbBuckets) {  // TODO move to an evaluation module
    fimpera<countingBF::CBF> f = fimpera<countingBF::CBF>(indexFilename);
    fimpera<TruthInTheShapeOfAnAMQ> fTruth = fimpera<TruthInTheShapeOfAnAMQ>(KMCFilename, f.getK(), 0, f.getCanonical(), size, nbBuckets);

    ResultGetter result_getter = ResultGetter();
    ResultGetter result_getter_truth = ResultGetter();

    f.query(queryFile, result_getter);
    fTruth.query(queryFile, result_getter_truth);

    std::vector<int> amq = result_getter.getResult();
    std::vector<int> truth = result_getter_truth.getResult();

    const auto& [tp, tn, fp, fn] = compareVectors(amq, truth);

    std::cout << tp << " " << tn << " " << fp << " " << fn << std::endl;
    std::cout << "fpr = " << ((double)fp) / ((double)(fp + tn)) << std::endl;

    toFileTXT("histo.txt", getHistogram(amq, truth));
}

int main(int argc, char* argv[]) {
    cxxopts::ParseResult arguments = parseArgvQuery(argc, argv);
    const auto& [index_filename, query_filename, K, z] = getArgsQuery(arguments);
    std::string KMCFilename = "/home/lrobidou/Documents/programmes/forge/fimperaBF/data/35merlists.txt";
    compareWithTruth(index_filename, KMCFilename, query_filename, 1, 1);  //TODO copy data from index ?
}