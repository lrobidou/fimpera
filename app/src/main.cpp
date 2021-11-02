#include <assert.h>  //TODO remove
#include <math.h>    // TODO remove ?
#include <robin_hood.h>

#include <fimpera-lib/CBF.hpp>
#include <fimpera-lib/fimpera.hpp>
#include <iostream>

#include "args.hpp"

// TODO mandatory desc with:
// name, version, git link

// TODO check uuid

class ResultGetter : public CustomResponse {
   private:
    std::vector<int> entireResponse;

   public:
    ResultGetter() {}

    void
    processResult(const std::vector<int>& res, const unsigned int& K, const std::string& current_header, const std::string& current_read) {
        // std::cout << current_header << std::endl;
        // for (const auto& x : res) {
        //     std::cout << x << " ";
        // }
        // std::cout << std::endl;
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
            if (data > _limitValueInBucket) {
                data = _limitValueInBucket;
            }
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

int main(int argc, char* argv[]) {
    int nbBuckets = 1;
    int size = 19495706 * nbBuckets;  // 19495706 -> 5%
    int K = 35;
    int z = 5;  // bucket 1 0.000467051%,  5: 0.000467051%
    std::string queryFile = "tests/unit/data/1millionLinesTest.txt";
    fimpera<countingBF::CBF> f = fimpera<countingBF::CBF>(queryFile, K, z, false, size, nbBuckets);
    // fimpera<TruthInTheShapeOfAnAMQ> fTruth = fimpera<TruthInTheShapeOfAnAMQ>(queryFile, K, z, false, size, nbBuckets);

    // f.queread("AAAAAAAACAGCGTTATCCGGAGATGAAGAAAGCA", result_getter);

    // ResultGetter result_getter = ResultGetter();
    // ResultGetter result_getter_truth = ResultGetter();

    // f.query("data/bio/ecoli1.fasta", result_getter);
    // // f.save("test.idx");
    // fTruth.query("data/bio/ecoli1.fasta", result_getter_truth);

    // std::vector<int> amq = result_getter.getResult();
    // std::vector<int> truth = result_getter_truth.getResult();

    // assert(amq.size() == truth.size());

    // int tp = 0;
    // int tn = 0;
    // int fp = 0;
    // int fn = 0;

    // for (int i = 0; i < amq.size(); i++) {
    //     if (truth[i]) {
    //         if (amq[i]) {
    //             tp++;
    //         } else {
    //             fn++;
    //         }
    //     } else {
    //         if (amq[i]) {
    //             fp++;
    //         } else {
    //             tn++;
    //         }
    //     }
    // }
    // std::cout << "fpr= " << ((double)fp) / ((double)(fp + tn)) << std::endl;
    f.save("save.idx");
    fimpera<countingBF::CBF> f2 = fimpera<countingBF::CBF>("save.idx");
    std::cout << (f == f2) << std::endl;
    // char[] msg = "bonjour";
    // std::cout << strlen(msg) << std::endl;

    return 0;
}