#pragma once
#include <fimpera-lib/fimpera.hpp>
#include <zstr.hpp>  //TODO
// #include <zstd.hpp>

#include <fimpera-lib/finderec.hpp>
#include <fimpera-lib/generators/ReadReader.hpp>

#include "files.hpp"

template <typename T>
inline void printVector(T x) {
    for (auto const& i : std::as_const(x)) {
        std::cout << i << ' ';
    }
    std::cout << std::endl;
}

template <typename T>
fimpera<T>::fimpera(const std::string& filename, const unsigned int& K, const unsigned int& z, bool canonical, uint64_t nbBits, uint64_t nbBuckets) : _filter(nbBits, nbBuckets), _canonical(canonical), _k(K - z), _z(z) {
    assert(K >= z);
    if (!fileExists(filename)) {
        std::cout << "The file " << filename << " does not exist." << std::endl;
        exit(1);
    }
    std::ifstream myFileGz(filename);
    zstr::istream myFile(myFileGz);

    std::string line;
    while (std::getline(myFile, line)) {  // \n are removed
        std::string Kmer;
        std::string abundanceStr;
        std::stringstream linestream(line);
        std::getline(linestream, Kmer, '\t');
        std::getline(linestream, abundanceStr, '\t');

        assert(Kmer.length() == K);
        unsigned long long size = Kmer.size();
        unsigned long long j = 0;  // start of the kmer in the Kmer
        while (j < size - _k + 1) {
            // TODO handle canonical
            _filter.set(Kmer.substr(j, _k), std::stoll(abundanceStr));
            j++;
        }
    }
}

template <typename T>
fimpera<T>::fimpera(const std::string& filename) {
    std::ifstream fin(filename, std::ios::out | std::ofstream::binary);
    std::string thisuuid = getFromFile<std::string>(fin);  //TODO check uuid
    std::string desc = getFromFile<std::string>(fin);
    _k = getFromFile<unsigned int>(fin);
    _z = getFromFile<unsigned int>(fin);
    _canonical = getFromFile<bool>(fin);
    std::string jsonStr = getFromFile<std::string>(fin);
    _filter = T(fin);
}

template <typename T>
void fimpera<T>::query(const std::string& filename, CustomResponse& response) const {
    FileManager reader = FileManager();
    reader.addFile(filename);
    std::string current_read;

    while (!(current_read = reader.get_next_read()).empty()) {
        std::string current_data = reader.get_data();
        std::string current_header = current_data.substr(0, current_data.find('\n'));
        std::vector<int> res = finderec(_filter, current_read, _k + _z, _z, _canonical);
        response.processResult(res, _k + _z, current_header, current_read);
    }
}

template <typename T>
void fimpera<T>::save(const std::string& filename) const {
    std::string jsonStr = "";
    // open the file
    std::ofstream fout(filename, std::ios::out | std::ofstream::binary);
    // write metadata
    writeToFile(fout, fimpera<T>::uuid0, fimpera<T>::description, _k, _z, _canonical, jsonStr);
    // write the filter
    _filter.dump(fout);
    // flush
    fout.flush();
    // close
    fout.close();
}

template <typename T>
void fimpera<T>::printMetadata(const std::string& filename) {
    std::ifstream fin(filename, std::ios::out | std::ofstream::binary);
    std::string thisuuid = getFromFile<std::string>(fin);  //TODO check uuid
    std::string description = getFromFile<std::string>(fin);
    unsigned int k = getFromFile<unsigned int>(fin);
    unsigned int z = getFromFile<unsigned int>(fin);
    bool canonical = getFromFile<bool>(fin);
    std::string jsonString = getFromFile<std::string>(fin);

    std::cout << "Reading index " << filename << ":" << std::endl;
    std::cout << "    " << description << std::endl;
    std::cout << "    uuid: " << thisuuid << std::endl;
    std::cout << "    k: " << k << std::endl;
    std::cout << "    z: " << z << std::endl;
    // std::cout << "    other: " << thisuuid << std::endl;  //TODO
}

template <typename T>
bool fimpera<T>::getCanonical() const {
    return this->_canonical;
}

template <typename T>
unsigned int fimpera<T>::getK() const {
    return this->_k + this->_z;
}

template <typename T>
unsigned int fimpera<T>::getz() const {
    return this->_z;
}

template <typename T>
fimpera<T>::~fimpera() {}