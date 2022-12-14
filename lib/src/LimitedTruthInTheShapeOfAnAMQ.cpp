#include <fimpera-lib/evaluation/LimitedTruthInTheShapeOfAnAMQ.hpp>

// class TruthInTheShapeOfAnAMQ {
//    private:
//     uint64_t _nbBuckets;
//     uint64_t _nbCells;
//     uint64_t _limitValueInBucket;
//     robin_hood::unordered_map<std::string, int> _t;

//    protected:
//     int get(const std::string& x) const {
//         auto it = _t.find(x);
//         if (it != _t.end()) {
//             return it->second;
//         } else {
//             return 0;
//         }
//     }

//    public:
//     TruthInTheShapeOfAnAMQ(int nbBits, int nbBuckets) : _nbBuckets(nbBuckets) {
//         _nbCells = nbBits / _nbBuckets;
//         _limitValueInBucket = pow(2, _nbBuckets) - 1;
//     }

//     bool set(const std::string& x, int occurrence = 1) {
//         // get existing data
//         int data = get(x);

//         // compute new value of data
//         if (data < occurrence) {
//             data = occurrence;
//         }

//         auto it = _t.find(x);
//         if (it != _t.end()) {
//             it->second = data;
//         } else {
//             _t.insert({x, data});
//         }
//         return data;
//     }
// };

#include <math.h>

// #include <fimpera-lib/evaluation/LimitedTruthInTheShapeOfAnAMQ.hpp>

int LimitedTruthInTheShapeOfAnAMQ::get(const std::string& x) const {
    auto it = _t.find(x);
    if (it != _t.end()) {
        return it->second;
    } else {
        return 0;
    }
}

LimitedTruthInTheShapeOfAnAMQ::LimitedTruthInTheShapeOfAnAMQ(int nbBits, int nbBuckets, int nbHash) : _nbBuckets(nbBuckets) {
    _nbCells = nbBits / _nbBuckets;
    _limitValueInBucket = pow(2, _nbBuckets) - 1;  // OPTIMIZE ?
}

bool LimitedTruthInTheShapeOfAnAMQ::set(const std::string& x, int occurrence) {
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

std::tuple<std::vector<std::size_t>, std::size_t> LimitedTruthInTheShapeOfAnAMQ::getStats() const {
    std::vector<std::size_t> v(_limitValueInBucket + 1, 0);

    for (const auto& [key, value] : _t) {
        v[value] += 1;
    }
    return {v, _nbCells};
}
