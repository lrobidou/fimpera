#include <assert.h>
#include <math.h>  // TODO remove ?
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

int CBF::get(const uint64_t& index) const {
    int data = 0;
    for (int i = 0; i < _nbBuckets; i++) {
        data <<= 1;
        data += _bits[index * _nbBuckets + i];  // TODO gros con
    }
    return data;
}

CBF::CBF(int nbBits, int nbBuckets) : _bits(nbBits, false), _nbBuckets(nbBuckets) {
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

int CBF::set(const std::string& kmer, int occurrence) {
    uint64_t index = getIndex(kmer);

    // get existing data
    int data = get(index);

    // compute new value of data
    if (data < occurrence) {
        data = occurrence;
        if (data > _limitValueInBucket) {
            data = _limitValueInBucket;
        }
    }

    // save data
    std::vector<bool> tmp(_nbBuckets);
    for (int i = 0; i < _nbBuckets; i++) {
        tmp[i] = data % 2;
        data >>= 1;
    }
    int start = index * _nbBuckets;
    for (int i = 0; i < _nbBuckets; i++) {
        _bits[start + i] = tmp[_nbBuckets - 1 - i];
    }

    return data;
}

int CBF::get(const std::string& kmer) const {
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
