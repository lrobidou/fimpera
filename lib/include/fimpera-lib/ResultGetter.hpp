#pragma once

#include <fimpera-lib/CustomResponse.hpp>
#include <tuple>
#include <vector>

class ResultGetter : public CustomResponse {
   private:
    std::vector<std::vector<int>> entireResponse;
    std::vector<std::vector<std::string>> kmers_reads;

   public:
    ResultGetter();

    void processResult(const std::vector<int>& res, const unsigned int& K, const std::string& current_header, const std::string& current_read);

    std::tuple<std::vector<std::vector<int>>, std::vector<std::vector<std::string>>> getResult();
};