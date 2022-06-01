#include <fimpera-lib/CBF.hpp>
#include <fimpera-lib/ResultGetter.hpp>
#include <fimpera-lib/fimpera.hpp>

#include "args.hpp"

class BiologicalDataPrinter : public CustomResponse {
    void processResult(const std::vector<int>& res, const unsigned int& K, const std::string& current_header, const std::string& current_read) {
        std::cout << current_header << '\n';
        std::size_t i = 0;

        const auto [minip, maxip] = std::minmax_element(begin(res), end(res));
        int mini = *minip;
        int maxi = *maxip;

        int max_size_of_integer_in_string;
        if (maxi > 0) {
            max_size_of_integer_in_string = trunc(log10(maxi));
        } else {
            max_size_of_integer_in_string = 1;
        }

        int sepsize = max_size_of_integer_in_string + 1;
        if (mini < 0) {
            max_size_of_integer_in_string += 1;
            sepsize++;
        }
        std::string pos_sep = std::string(sepsize, ' ');
        // std::string neg_sep = std::string(max_size_of_integer_in_string, ' ');

        while (i < current_read.size()) {
            std::cout << current_read[i] << pos_sep;
            i++;
        }
        std::cout << '\n';
        for (auto x : res) {
            int sepsize_x;
            if (x < 0) {
                sepsize_x = max_size_of_integer_in_string - trunc(log10(-x)) - 1;
                // std::cout << std::endl
                //   << sepsize_x << std::endl;
            } else if (x == 0) {
                sepsize_x = max_size_of_integer_in_string;
            } else {
                sepsize_x = max_size_of_integer_in_string - trunc(log10(x));
            }

            std::string sep = std::string(sepsize_x + 1, ' ');
            std::cout << x << sep;
        }
        std::cout << std::endl;
    }
};

int main(int argc, char* argv[]) {
    argparse::ArgumentParser program("fimpera_index", "0.0.1");

    program.add_argument("input_filename").help("index you want to query");
    program.add_argument("query_filename").help("file you want to query against the index");

    parse(program, argc, argv);

    const std::string index_filename = program.get("input_filename");
    const std::string query_filename = program.get("query_filename");

    fimpera<countingBF::CBF> f = fimpera<countingBF::CBF>(index_filename);
    BiologicalDataPrinter printer = BiologicalDataPrinter();
    f.query(query_filename, printer);
}