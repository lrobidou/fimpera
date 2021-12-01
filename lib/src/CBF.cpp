#include <assert.h>
#include <math.h>  // OPTIMIZE use << instead ?
#include <xxhash64.h>

#include <cstring>
#include <fimpera-lib/CBF.hpp>
#include <fimpera-lib/files.hpp>
#include <iostream>

namespace countingBF {

uint64_t CBF::getIndex(const std::string& kmer) const {
    const char* kmerCopy = kmer.c_str();
    uint64_t hashedKmer = XXHash64::hash((void*)kmerCopy, strlen(kmerCopy), 0);
    uint64_t index = hashedKmer % _nbCells;
    return index;
}

uint64_t CBF::get(const uint64_t& index) const {
    uint64_t data = 0;
    for (uint64_t i = 0; i < _nbBuckets; i++) {
        data <<= 1;
        data += _bits[index * _nbBuckets + i];
    }
    return data;
}

CBF::CBF(uint64_t nbBits, uint64_t nbBuckets) : _bits(nbBits, false), _nbBuckets(nbBuckets) {
    assert(nbBits > 0);
    _nbCells = nbBits / _nbBuckets;
    _limitValueInBucket = pow(2, _nbBuckets) - 1;
}

CBF::CBF(std::ifstream& fin) {
    _nbBuckets = getFromFile<uint64_t>(fin);
    _nbCells = getFromFile<uint64_t>(fin);
    _limitValueInBucket = pow(2, _nbBuckets) - 1;

    std::vector<bool>::size_type n;
    fin.read((char*)&n, sizeof(std::vector<bool>::size_type));
    _bits.resize(n);
    for (std::vector<bool>::size_type i = 0; i < n;) {
        unsigned char aggr;
        fin.read((char*)&aggr, sizeof(unsigned char));
        for (unsigned char mask = 1; mask > 0 && i < n; ++i, mask <<= 1)
            _bits.at(i) = aggr & mask;
    }
}

uint64_t CBF::set(const std::string& kmer, uint64_t occurrence) {
    uint64_t index = getIndex(kmer);

    // get existing data
    uint64_t data = get(index);

    // compute new value of data
    if (data < occurrence) {
        data = occurrence;
        if (data > _limitValueInBucket) {
            data = _limitValueInBucket;
        }
    }

    // save data
    std::vector<bool> tmp(_nbBuckets);
    for (uint64_t i = 0; i < _nbBuckets; i++) {
        tmp[i] = data % 2;
        data >>= 1;
    }
    uint64_t start = index * _nbBuckets;
    for (uint64_t i = 0; i < _nbBuckets; i++) {
        _bits[start + i] = tmp[_nbBuckets - 1 - i];
    }

    return data;
}

uint64_t CBF::get(const std::string& kmer) const {
    return get(getIndex(kmer));
}

void CBF::dump(std::ofstream& fout) const {
    writeToFile(fout, _nbBuckets, _nbCells, _bits);
    fout.flush();
}

bool CBF::operator==(const CBF& that) const {
    return (
        (this->_nbBuckets == that._nbBuckets) &&
        (this->_nbCells == that._nbCells) &&
        (this->_limitValueInBucket == that._limitValueInBucket) &&
        (this->_bits == that._bits));
}

CBF::~CBF() {}

}  // namespace countingBF
