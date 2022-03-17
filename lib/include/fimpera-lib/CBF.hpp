#pragma once

#include <fstream>
#include <string>
#include <tuple>
#include <vector>
/**
    @file ex_lib.hpp
*/

namespace countingBF {
class CBF {
   private:
    std::vector<bool> _bits;  // bits used by the counting bloom filter
    uint64_t _nbBitsPerCell;
    uint64_t _nbCells;
    uint64_t _nbHashFunctions;
    uint64_t _limitValueInBucket;
    static const std::string _uuid;
    std::vector<uint64_t> getIndexes(const std::string& kmer) const;
    uint64_t get(const uint64_t& index) const;
    std::tuple<std::vector<uint64_t>, uint64_t> get(const std::vector<uint64_t>& indexes) const;
    // uint64_t get(const std::vector<uint64_t>& indexes) const;

   public:
    CBF(uint64_t nbBits = 100, uint64_t nbBitsPerCell = 1, uint64_t nbHashFunctions = 1);
    CBF(std::ifstream& fin);
    uint64_t set(const std::string& kmer, uint64_t occurrence = 1);
    uint64_t get(const std::string& kmer) const;
    void dump(std::ofstream& fout) const;
    bool operator==(const CBF& that) const;

    std::vector<std::size_t> getStats() const;
    ~CBF();
};

}  // namespace countingBF