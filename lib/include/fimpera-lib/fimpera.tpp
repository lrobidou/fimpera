#pragma once
#include <fimpera-lib/fimpera.hpp>

#include "../../../thirdparty/zstr/src/zstr.hpp"  //TODO
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
fimpera<T>::fimpera(const std::string& filename, const unsigned int& K, const unsigned int& z, bool canonical, int nbBits, int nbBuckets) : _filter(nbBits, nbBuckets), _canonical(canonical), _k(K - z), _z(z) {
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

    std::string thuisuuid = getFromFile<std::string>(fin);
    std::string desc = getFromFile<std::string>(fin);
    _k = getFromFile<unsigned int>(fin);
    _z = getFromFile<unsigned int>(fin);
    _canonical = getFromFile<bool>(fin);
    _filter = T(fin);

    // std::cout << thuisuuid << std::endl;
    // std::cout << desc << std::endl;
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
    std::string desc = "fimpera index version 0. は. ";  //TODO move

    // open the file
    std::ofstream fout(filename, std::ios::out | std::ofstream::binary);

    writeToFile(fout, fimpera<T>::uuid0, desc, _k, _z, _canonical);
    _filter.dump(fout);  // write the filter

    fout.flush();
    fout.close();
}

template <typename T>
fimpera<T>::~fimpera() {}