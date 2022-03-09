#pragma once

#include <stdint.h>

#include <cmath>
#include <iostream>

namespace abundanceToIdentifierStrategy {

class identity {
   public:
    static uint64_t fct(const uint64_t& abundance);
    static std::string name();
};

class log2 {
   public:
    static uint64_t fct(const uint64_t& abundance);
    static std::string name();
};

}  // namespace abundanceToIdentifierStrategy
