#include <assert.h>
#include <math.h>  // OPTIMIZE use << instead ?
#include <xxhash64.h>

#include <cstring>
#include <fimpera-lib/CBF.hpp>
#include <fimpera-lib/files.hpp>
#include <iostream>

namespace countingBF {

const std::string CBF::_uuid = "7c1b484b-3b9a-4b3e-9534-56d18796f69a";

std::vector<uint64_t> CBF::getIndexes(const std::string& kmer) const {
    std::vector<uint64_t> indexes;
    const char* kmerCopy = kmer.c_str();
    for (uint64_t i = 0; i < _nbHashFunctions; i++) {
        uint64_t hashedKmer = XXHash64::hash((void*)kmerCopy, strlen(kmerCopy), i);
        uint64_t index = hashedKmer % _nbCells;
        indexes.push_back(index);
    }
    return indexes;
}

uint64_t CBF::get(const uint64_t& index) const {
    uint64_t valueInFilter = 0;
    for (uint64_t i = 0; i < _nbBitsPerCell; i++) {
        valueInFilter <<= 1;
        valueInFilter += _bits[index * _nbBitsPerCell + i];
    }
    return valueInFilter;
}

std::tuple<std::vector<uint64_t>, uint64_t> CBF::get(const std::vector<uint64_t>& indexes) const {
    std::vector<uint64_t> valuesInFilter;
    valuesInFilter.reserve(indexes.size());
    uint64_t minValue = std::numeric_limits<uint64_t>::max();

    for (const auto& index : indexes) {
        uint64_t data = get(index);
        valuesInFilter.push_back(data);
        if (minValue > data) {
            minValue = data;
        }
    }
    return {valuesInFilter, minValue};
}

CBF::CBF(uint64_t nbBits, uint64_t nbBitsPerCell, uint64_t nbHashFunctions) : _bits(nbBits, false), _nbBitsPerCell(nbBitsPerCell), _nbHashFunctions(nbHashFunctions) {
    assert(nbBits > 0);
    _nbCells = nbBits / _nbBitsPerCell;
    _limitValueInBucket = pow(2, _nbBitsPerCell) - 1;
}

CBF::CBF(std::ifstream& fin) {
    std::string uuidStored = getFromFile<std::string>(fin);
    // if the uuid in the stream is not the same as the one I know, raise an exception
    // this allows to manage version and to prevent reading an incorrect stream
    if (uuidStored != CBF::_uuid) {
        std::string msg = "Invalid uuid found when reading index. UUID found is : " + uuidStored + " . Supported UUID is " + _uuid + ". Your are likely using an old version of this software to read an index made by a newer version. Please update or report bug if this keep happening.\n(UUIDs are a protection against version mismatch and corrupted indexes. At the begining of development, this software did not support UUIDs, so no UUID was written in indexes, and as such first versions are incompatible with the newest ones. Do not use those WIP versions, ie. versions before paper submission.)";
        throw std::runtime_error(msg);
    }
    // now, we know that the stream contains data compatible with us
    // let's proceed to read it
    _nbBitsPerCell = getFromFile<uint64_t>(fin);
    _nbCells = getFromFile<uint64_t>(fin);
    _nbHashFunctions = getFromFile<uint64_t>(fin);

    _limitValueInBucket = pow(2, _nbBitsPerCell) - 1;

    std::vector<bool>::size_type n;
    fin.read((char*)&n, sizeof(std::vector<bool>::size_type));
    _bits.resize(n);
    // this part is ugly, but it works (tm)
    // basically, iterate through the data on stream byte by byte and add it in the filter bit by bit
    std::vector<bool>::size_type i = 0;
    while (i < n) {
        // get the next byte from the stream
        unsigned char aggr;
        fin.read((char*)&aggr, sizeof(unsigned char));
        // we have a byte to store in the filter
        // let's select its bit one by one and place them in the filter
        for (unsigned char mask = 1; mask > 0 && i < n; ++i, mask <<= 1)
            _bits.at(i) = aggr & mask;
    }
}

// store the filter in the filter and returns the value stored
uint64_t CBF::set(const std::string& kmer, uint64_t occurrence) {
    // can't set values higher because of space limit
    if (occurrence > _limitValueInBucket) {
        occurrence = _limitValueInBucket;
    }

    // retrive indexes and values from the filter
    std::vector<uint64_t> indexes = getIndexes(kmer);
    const auto& [valuesInFilter, minValue] = get(indexes);

    // If the value we want to store is lower than what's already stored in the
    // filter, then storing won't change the filter's state.
    // In such case, we can end the function now.
    if (minValue >= occurrence) {
        return minValue;
    }

    for (uint64_t i = 0; i < indexes.size(); ++i) {
        uint64_t index = indexes[i];
        uint64_t valueInFilterForThatIndex = valuesInFilter[i];
        if (valueInFilterForThatIndex < occurrence) {
            valueInFilterForThatIndex = occurrence;
        }

        // save data
        // first, convert a uint64_t to a std::vector<bool>
        std::vector<bool> tmp(_nbBitsPerCell);
        for (uint64_t i = 0; i < _nbBitsPerCell; i++) {
            tmp[i] = valueInFilterForThatIndex % 2;
            valueInFilterForThatIndex >>= 1;
        }

        // then, save this vector in the _bits vector
        uint64_t start = index * _nbBitsPerCell;
        for (uint64_t j = 0; j < _nbBitsPerCell; j++) {
            _bits[start + j] = tmp[_nbBitsPerCell - 1 - j];
        }
    }
    // OPTIMIZE convert from uint64 to vector directly in the _bits vector (spares one memory allocation per insertion)

    return occurrence;
}

uint64_t CBF::get(const std::string& kmer) const {
    return std::get<1>(get(getIndexes(kmer)));
}

void CBF::dump(std::ofstream& fout) const {
    writeToFile(fout, CBF::_uuid, _nbBitsPerCell, _nbCells, _nbHashFunctions, _bits);
    fout.flush();
}

bool CBF::operator==(const CBF& that) const {
    return (
        (this->_nbBitsPerCell == that._nbBitsPerCell) &&
        (this->_nbCells == that._nbCells) &&
        (this->_limitValueInBucket == that._limitValueInBucket) &&
        (this->_bits == that._bits));
}

// returns a vector such that vector[i] = len([x for x in vector if x == i])
std::vector<std::size_t> CBF::getStats() const {
    std::vector<std::size_t> v(_limitValueInBucket + 1, 0);

    for (std::size_t i = 0; i < _nbCells; i++) {
        v[get(i)] += 1;
    }
    return v;
}

CBF::~CBF() {}

}  // namespace countingBF
