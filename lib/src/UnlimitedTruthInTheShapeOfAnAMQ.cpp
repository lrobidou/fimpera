
#include <math.h>

#include <fimpera-lib/evaluation/UnlimitedTruthInTheShapeOfAnAMQ.hpp>

int UnlimitedTruthInTheShapeOfAnAMQ::get(const std::string& x) const {
    auto it = _t.find(x);
    if (it != _t.end()) {
        return it->second;
    } else {
        return 0;
    }
}

UnlimitedTruthInTheShapeOfAnAMQ::UnlimitedTruthInTheShapeOfAnAMQ(int nbBits, int nbBuckets) : _nbBuckets(nbBuckets) {
    _nbCells = nbBits / _nbBuckets;
    _limitValueInBucket = pow(2, _nbBuckets) - 1;  // OPTIMIZE ?
}

bool UnlimitedTruthInTheShapeOfAnAMQ::set(const std::string& x, int occurrence) {
    // get existing data
    int data = get(x);

    // compute new value of data
    if (data < occurrence) {
        data = occurrence;
    }

    auto it = _t.find(x);
    if (it != _t.end()) {
        it->second = data;
    } else {
        _t.insert({x, data});
    }
    return data;
}

std::tuple<std::vector<std::size_t>, std::size_t> UnlimitedTruthInTheShapeOfAnAMQ::getStats() const {
    std::vector<std::size_t> v(_limitValueInBucket + 1, 0);

    for (const auto& [key, value] : _t) {
        v[value] += 1;
    }
    return {v, _nbCells};
}