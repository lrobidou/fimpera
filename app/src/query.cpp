#include <fimpera-lib/CBF.hpp>
#include <fimpera-lib/fimpera.hpp>

#include "args/query_args.hpp"

class ResultGetter : public CustomResponse {
   private:
    std::vector<int> entireResponse;

   public:
    ResultGetter() {}

    void processResult(const std::vector<int>& res, const unsigned int& K, const std::string& current_header, const std::string& current_read) {
        entireResponse.insert(std::end(entireResponse), std::begin(res), std::end(res));
    }

    std::vector<int> getResult() {
        return entireResponse;
    }
};

int main(int argc, char* argv[]) {
    cxxopts::ParseResult arguments = parseArgvQuery(argc, argv);
    const auto& [index_filename, query_filename, K, z] = getArgsQuery(arguments);
    std::string KMCFilename = "/home/lrobidou/Documents/programmes/forge/fimperaBF/data/35merlists.txt";

    fimpera<countingBF::CBF> f = fimpera<countingBF::CBF>(index_filename);
    ResultGetter result_getter = ResultGetter();
    f.query(query_filename, result_getter);
    std::vector<int> amq = result_getter.getResult();
}