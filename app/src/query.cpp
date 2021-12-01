#include <fimpera-lib/CBF.hpp>
#include <fimpera-lib/fimpera.hpp>

#include "args.hpp"

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
    argparse::ArgumentParser program("fimpera_index", "0.0.1");
    // mandatory arguments
    program.add_argument("input_filename").help("index you want to query");
    program.add_argument("query_filename").help("file you want to query against the index");

    parse(program, argc, argv);
    // optional arguments
    // TODO use value stored in filter
    // program.add_argument("-K").help("size of Kmers").default_value(31).scan<'i', int>();
    // program.add_argument("-z").help("value of z (cf paper of findere)").default_value(3).scan<'i', int>();

    const std::string index_filename = program.get("input_filename");
    const std::string query_filename = program.get("query_filename");
    // const int K = program.get<int>("-K");//TODO
    // const int z = program.get<int>("-z");

    fimpera<countingBF::CBF> f = fimpera<countingBF::CBF>(index_filename);
    ResultGetter result_getter = ResultGetter();
    f.query(query_filename, result_getter);
    std::vector<int> amq = result_getter.getResult();
}