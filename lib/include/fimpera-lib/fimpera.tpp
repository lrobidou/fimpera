#pragma once

#include <fimpera-lib/canonical.hpp>
#include <fimpera-lib/fimpera.hpp>
#include <fimpera-lib/finderec.hpp>
#include <fimpera-lib/generators/ReadReader.hpp>
#include <zstr.hpp>

template <typename T>
fimpera<T>::fimpera(const std::string& filename, const int& K, const int& z, bool canonical, uint64_t nbBits, uint64_t nbBuckets) : _filter(nbBits, nbBuckets), _canonical(canonical), _k(K - z), _z(z) {
    assert(K >= z);
    if (!fileExists(filename)) {
        std::string msg = "The file " + filename + " does not exist.";
        throw std::runtime_error(msg);
    }
    std::ifstream myFileGz(filename);
    zstr::istream myFile(myFileGz);

    std::string line;
    std::string kmer;
    while (std::getline(myFile, line)) {  // \n are removed
        std::string Kmer;
        std::string abundanceStr;
        std::stringstream linestream(line);
        std::getline(linestream, Kmer, '\t');
        std::getline(linestream, abundanceStr, '\t');

        // assert(Kmer.length() == K);//TODO check that the file actually contains kmer (or smer)
        unsigned long long size = Kmer.size();
        unsigned long long j = 0;  // start of the kmer in the Kmer
        while (j + _k < size + 1) {
            kmer = Kmer.substr(j, _k);
            if (_canonical) {
                kmer = toCanonical(kmer);
            }
            _filter.set(kmer, std::stoll(abundanceStr));
            j++;
        }
    }
}

template <typename T>
fimpera<T>::fimpera(const std::string& filename) {
    std::ifstream fin(filename, std::ios::out | std::ofstream::binary);
    std::string thisuuid = getFromFile<std::string>(fin);  // TODO check uuid
    std::string desc = getFromFile<std::string>(fin);
    _k = getFromFile<unsigned int>(fin);
    _z = getFromFile<unsigned int>(fin);
    _canonical = getFromFile<bool>(fin);
    std::string jsonStr = getFromFile<std::string>(fin);
    _filter = T(fin);
}

template <typename T>
void fimpera<T>::query(const std::string& filename, CustomResponse& response) const {
    for (const auto& [read, header] : fimpera_lib::generators::ReadReader(filename)) {
        std::vector<int> res = finderec(_filter, read, _k + _z, _z, _canonical);
        response.processResult(res, _k + _z, header, read);
    }
}

template <typename T>
std::vector<int> fimpera<T>::queryRead(const std::string& read) const {
    return finderec(_filter, read, _k + _z, _z, _canonical);
}

template <typename T>
void fimpera<T>::save(const std::string& filename) const {
    std::string jsonStr = "";  // TODO add infos of the data structure used to index
    // TODO make the T have a method to retrieve inner info and add them to the json
    // open the file
    std::ofstream fout(filename, std::ios::out | std::ofstream::binary);
    // write metadata
    writeToFile(fout, fimpera_uuid0, fimpera<T>::description, _k, _z, _canonical, jsonStr);
    // write the filter
    _filter.dump(fout);
    // flush
    fout.flush();
    // close
    fout.close();
}

template <typename T>
bool fimpera<T>::getCanonical() const {
    return this->_canonical;
}

template <typename T>
int fimpera<T>::getK() const {
    return this->_k + this->_z;
}

template <typename T>
int fimpera<T>::getz() const {
    return this->_z;
}

template <typename T>
fimpera<T>::~fimpera() {}