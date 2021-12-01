#pragma once

#include <fstream>
#include <string>
#include <vector>
/**
    @file ex_lib.hpp
*/

namespace countingBF {
class CBF {
   private:
    std::vector<bool> _bits;
    uint64_t _nbBuckets;
    uint64_t _nbCells;
    uint64_t _limitValueInBucket;
    uint64_t getIndex(const std::string& kmer) const;
    uint64_t get(const uint64_t& index) const;

   public:
    CBF(uint64_t nbBits = 100, uint64_t nbBuckets = 1);
    CBF(std::ifstream& fin);
    uint64_t set(const std::string& kmer, uint64_t occurrence = 1);
    uint64_t get(const std::string& kmer) const;
    void dump(std::ofstream& fout) const;
    bool operator==(const CBF& that) const;
    ~CBF();
};

}  // namespace countingBF