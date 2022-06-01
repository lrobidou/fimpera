#pragma once
#include <string>
#include <vector>

class CustomResponse {
   public:
    CustomResponse() = default;
    ~CustomResponse() = default;
    virtual void processResult(const std::vector<int>& res, const unsigned int& K, const std::string& current_header, const std::string& current_read) = 0;
};
