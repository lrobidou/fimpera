#pragma once

#include <string>
#include <tuple>

#include "CustomResponse.hpp"
#include "files.hpp"

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

const std::string fimpera_uuid0 = "23e0132d-0e3d-4639-9794-69c2e22a0af4";

template <typename T>
const std::string fimpera<T>::description = "fimpera index version 0. https://github.com/lrobidou/fimpera";

inline std::string readUUID(std::ifstream& fin) {
    return getFromFile<std::string>(fin);  //TODO check uuid
}

inline std::tuple<std::string, unsigned int, unsigned int, bool, std::string> getMetadatav0(std::ifstream& fin) {
    std::string description = getFromFile<std::string>(fin);
    unsigned int k = getFromFile<unsigned int>(fin);
    unsigned int z = getFromFile<unsigned int>(fin);
    bool canonical = getFromFile<bool>(fin);
    std::string jsonString = getFromFile<std::string>(fin);
    return {description, k, z, canonical, jsonString};
}

inline std::tuple<std::string, std::string, unsigned int, unsigned int, bool, std::string> getMetadata(std::ifstream& fin) {
    std::string uuid = readUUID(fin);
    if (uuid == fimpera_uuid0) {
        return std::tuple_cat(std::make_tuple(uuid), getMetadatav0(fin));
    } else {
        std::string msg = "Invalid uuid found when reading index. UUID is: " + uuid + " .";
        throw std::runtime_error(msg);
    }
}

#include "fimpera.tpp"