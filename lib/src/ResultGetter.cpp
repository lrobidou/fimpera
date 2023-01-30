#include <fimpera-lib/ResultGetter.hpp>
#include <iostream>

ResultGetter::ResultGetter() {}

void ResultGetter::processResult(const std::vector<int>& res, const unsigned int& k, const std::string& current_header, const std::string& current_read) {
    entireResponse.push_back(res);
    std::vector<std::string> kmers;
    std::size_t j = 0;
    std::size_t size = current_read.size();

    while (j < size - k + 1) {
        kmers.push_back(current_read.substr(j, k));
        j++;
    }
    if (kmers.size() != res.size()) {
        std::cerr << "read.size() != res.size()" << kmers.size() << " " << res.size() << std::endl;
        exit(1);
    }
    kmers_reads.push_back(kmers);
}

std::tuple<std::vector<std::vector<int>>, std::vector<std::vector<std::string>>> ResultGetter::getResult() {
    return {entireResponse, kmers_reads};
}