#pragma once
#include <robin_hood.h>

#include <string>

class UnlimitedTruthInTheShapeOfAnAMQ {
   protected:
    uint64_t _nbBuckets;
    uint64_t _nbCells;
    uint64_t _limitValueInBucket;
    robin_hood::unordered_map<std::string, int> _t;

   public:
    UnlimitedTruthInTheShapeOfAnAMQ(int nbBits, int nbBuckets);

    bool set(const std::string& x, int occurrence = 1);
    int get(const std::string& x) const;
    std::tuple<std::vector<std::size_t>, std::size_t> getStats() const;
};