#pragma once

#include <iostream>  //TODO remove
#include <string>

#include "CustomResponse.hpp"
template <typename T>
class fimpera {
   private:
    T _filter;
    bool _canonical;
    unsigned int _k;
    unsigned int _z;
    const static std::string uuid0;
    const static std::string description;

   public:
    fimpera(const std::string& filename, const unsigned int& K, const unsigned int& z, bool canonical, uint64_t nbBits, uint64_t nbBuckets = 1);
    fimpera(const std::string& filename);
    void query(const std::string& filename, CustomResponse& response) const;
    void save(const std::string& filename) const;
    static void printMetadata(const std::string& filename);

    // getter
    bool getCanonical() const;
    unsigned int getK() const;
    unsigned int getz() const;

    // operator
    bool operator==(const fimpera<T>& that) const {
        return ((this->_k == that._k) && (this->_z == that._z) && (this->_canonical == that._canonical) && (this->_filter == that._filter));  // TODO should I check for z too ? for uuid ?
    }

    ~fimpera();
};

template <typename T>
const std::string fimpera<T>::uuid0 = "23e0132d-0e3d-4639-9794-69c2e22a0af4";

template <typename T>
const std::string fimpera<T>::description = "fimpera index version 0. https://github.com/lrobidou/fimpera";

#include "fimpera.tpp"