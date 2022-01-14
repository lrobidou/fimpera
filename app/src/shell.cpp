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

namespace shell {

class eof : public std::exception {
    virtual const char* what() const throw() {
        return "EOF";
    }
};

inline std::string input() {
    std::cout << " > ";
    std::string cmd = "";
    std::getline(std::cin, cmd);
    if (std::cin.eof()) {
        throw eof();
    }
    return cmd;
}
}  // namespace shell

int main(int argc, char* argv[]) {
    argparse::ArgumentParser program("fimpera_index", "0.0.1");
    // mandatory arguments
    program.add_argument("input_filename").help("index you want to query");

    parse(program, argc, argv);

    const std::string index_filename = program.get("input_filename");
    fimpera<countingBF::CBF> f = fimpera<countingBF::CBF>(index_filename);

    std::string cmd;
    try {
        while ((cmd = shell::input()) != "exit") {
            ResultGetter result_getter = ResultGetter();
            f.query_read(cmd, result_getter);
            std::vector<int> response = result_getter.getResult();
            for (const auto& x : response) {
                std::cout << x;
            }
            std::cout << std::endl;
        }
    } catch (const shell::eof& e) {
        return 0;
    }
}