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

   public:
    fimpera(const std::string& filename, const unsigned int& K, const unsigned int& z, bool canonical, int nbBits, int nbBuckets = 1);
    fimpera(const std::string& filename);
    void query(const std::string& filename, CustomResponse& response) const;
    void save(const std::string& filename) const;
    bool operator==(const fimpera<T>& that) const {
        return ((this->_k == that._k) && (this->_z == that._z) && (this->_canonical == that._canonical) && (this->_filter == that._filter));  // TODO should I check for z too ? for uuid ?
    }

    ~fimpera();
};

template <typename T>
const std::string fimpera<T>::uuid0 = "23e0132d-0e3d-4639-9794-69c2e22a0af4";

#include "fimpera.tpp"