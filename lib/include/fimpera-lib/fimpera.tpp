#pragma once

#include <fimpera-lib/abundanceToIdentifierStrategy.hpp>
#include <fimpera-lib/canonical.hpp>
#include <fimpera-lib/fimpera.hpp>
#include <fimpera-lib/finderec.hpp>
#include <fimpera-lib/generators/ReadReader.hpp>
#include <nlohmann/json.hpp>
#include <zstr.hpp>

// #include "abundanceToIdentifierStrategy.hpp"

inline void checkExists(const std::string& filename) {
    if (!fileExists(filename)) {
        std::string msg = "The file " + filename + " does not exist.";
        throw std::runtime_error(msg);
    }
}

template <typename T>
fimpera<T>::fimpera(const std::string& filename, const int& K, const int& z, bool canonical, uint64_t nbBits, uint64_t nbBuckets) : _filter(nbBits, nbBuckets), _canonical(canonical), _k(K - z), _z(z) {
    assert(K >= z);
    checkExists(filename);
    this->_strategy = "identity";
    // std::cout << "in fimpera constructor " << nbBits << std::endl; // TODO: remove
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

        try {
            uint64_t abundance = std::stoll(abundanceStr);
            uint64_t id = abundance;
            while (j + _k < size + 1) {
                kmer = Kmer.substr(j, _k);
                if (_canonical) {
                    kmer = toCanonical(kmer);
                }
                _filter.set(kmer, id);
                j++;
            }
        } catch (const std::invalid_argument& e) {
            std::cerr << "Invalid argument when parsing line :\"" << line << "\"" << std::endl;
        }
    }
}

template <typename T>
template <typename U>
fimpera<T>::fimpera(const U& strategyWrapper, const std::string& filename, const int& K, const int& z, bool canonical, uint64_t nbBits, uint64_t nbBuckets) : _filter(nbBits, nbBuckets), _canonical(canonical), _k(K - z), _z(z) {
    assert(K >= z);
    checkExists(filename);
    this->_strategy = strategyWrapper.name();
    // this->_strategy = "log2";

    // std::cout << "in fimpera constructor " << nbBits << std::endl; // TODO: remove
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

        try {
            uint64_t abundance = std::stoll(abundanceStr);
            uint64_t id = strategyWrapper.fct(abundance);
            while (j + _k < size + 1) {
                kmer = Kmer.substr(j, _k);
                if (_canonical) {
                    kmer = toCanonical(kmer);
                }
                _filter.set(kmer, id);
                j++;
            }
        } catch (const std::invalid_argument& e) {
            std::cerr << "Invalid argument when parsing line :\"" << line << "\"" << std::endl;
        }
    }
}

template <typename T>
template <typename U>
fimpera<T>::fimpera(const U& strategyWrapper, const std::string& filename, const int& K, const int& z, bool canonical, uint64_t nbBits) : _filter(nbBits, 1), _canonical(canonical), _k(K - z), _z(z) {
    assert(K >= z);
    checkExists(filename);
    this->_strategy = strategyWrapper.name();

    // std::cout << "in fimpera constructor " << nbBits << std::endl; // TODO: remove
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

        try {
            uint64_t abundance = std::stoll(abundanceStr);
            uint64_t id = strategyWrapper.fct(abundance);
            while (j + _k < size + 1) {
                kmer = Kmer.substr(j, _k);
                if (_canonical) {
                    kmer = toCanonical(kmer);
                }
                _filter.set(kmer, id);
                j++;
            }
        } catch (const std::invalid_argument& e) {
            std::cerr << "Invalid argument when parsing line :\"" << line << "\"" << std::endl;
        }
    }
}

template <typename T>
fimpera<T>::fimpera(const std::string& filename) {
    // std::cout << "loading fimpera from filename: " << filename << std::endl;//TODO log
    if (!fileExists(filename)) {
        std::string msg = "The file " + filename + " does not exist.";
        throw std::runtime_error(msg);
    }
    std::ifstream fin(filename, std::ios::out | std::ofstream::binary);
    std::string thisuuid = getFromFile<std::string>(fin);  // TODO check uuid
    std::string desc = getFromFile<std::string>(fin);
    _k = getFromFile<unsigned int>(fin);
    _z = getFromFile<unsigned int>(fin);
    _canonical = getFromFile<bool>(fin);

    // get the JSON back from the index
    std::string jsonStr = getFromFile<std::string>(fin);

    // get  the strategy back from the index
    // std::cout << j.at("strategy") << std::endl;

    if (jsonStr.length() != 0) {
        nlohmann::json j = nlohmann::json::parse(jsonStr);
        if (j.contains("strategy")) {
            this->_strategy = j["strategy"];
        } else {
            this->_strategy = "identity";
        }
    } else {
        this->_strategy = "identity";
    }

    _filter = T(fin);
}

template <typename T>
void fimpera<T>::query(const std::string& filename, CustomResponse& response) const {
    checkExists(filename);
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
    std::string jsonStr = "{\"strategy\":\"" + this->_strategy + "\"}";  // TODO add infos of the data structure used to index
    // TODO make the T have a method to retrieve inner info and add them to the json
    // remove(filename.c_str()); // todo ?
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
const T& fimpera<T>::getInnerFilter() const {
    return this->_filter;
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
std::string fimpera<T>::getStrategyName() const {
    return this->_strategy;
}

template <typename T>
fimpera<T>::~fimpera() {}
