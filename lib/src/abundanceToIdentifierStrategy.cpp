#include <stdint.h>

#include <cmath>
#include <fimpera-lib/abundanceToIdentifierStrategy.hpp>
#include <iostream>
namespace abundanceToIdentifierStrategy {

uint64_t identity::fct(const uint64_t& abundance) {
    return abundance;
}

std::string identity::name() {
    return "identity";
}

uint64_t log2::fct(const uint64_t& abundance) {
    if (abundance == 0) {
        std::cerr << "Tried to compute the log of 0 and failed ()." << std::endl;
        exit(1);
    }
    uint64_t res = ::log2(abundance) + 1;
    return res;
}

std::string log2::name() {
    return "log2";
}

}  // namespace abundanceToIdentifierStrategy
