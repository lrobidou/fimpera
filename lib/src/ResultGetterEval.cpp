#include <fimpera-lib/evaluation/ResultGetterEval.hpp>
#include <iostream>

ResultGetterEval::ResultGetterEval() {}

void ResultGetterEval::processResult(const std::vector<int>& res, const unsigned int& K, const std::string& current_header, const std::string& current_read) {
}

std::tuple<std::vector<std::vector<int>>, std::vector<std::vector<std::string>>> ResultGetterEval::getResult() {
    return {entireResponse, kmers_reads};
}