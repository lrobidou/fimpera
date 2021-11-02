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
    int get(const uint64_t& index) const;

   public:
    CBF(int nbBits = 100, int nbBuckets = 1);
    CBF(std::ifstream& fin);
    int set(const std::string& kmer, int occurrence = 1);
    int get(const std::string& kmer) const;
    void dump(std::ofstream& fout) const;
    bool operator==(const CBF& that) const;
    ~CBF();
};

}  // namespace countingBF