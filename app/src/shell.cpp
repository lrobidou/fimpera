#include <robin_hood.h>

#include <fimpera-lib/CBF.hpp>
#include <fimpera-lib/fimpera.hpp>
#include <memory>
#include <string>

#include "args.hpp"

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

class Memory;
using shell_method_t = void (Memory::*)(const std::string&);

class Memory {
   private:
    robin_hood::unordered_map<std::string, shell_method_t> _mapping{};
    robin_hood::unordered_map<std::string, std::unique_ptr<fimpera<countingBF::CBF>>> _filters{};

    void load(const std::string& cmd);

    void do_query(const std::string& filename, const std::unique_ptr<fimpera<countingBF::CBF>>& filter, const std::string& read) {
        std::cout << "quering " << filename << std::endl;

        std::string to_print = "";

        std::vector<int> response = filter->queryRead(read);
        for (const auto& x : response) {
            if (!to_print.empty()) {
                to_print += " ";
            }
            to_print += std::to_string(x);
        }
        std::cout << to_print << std::endl;
    }

   public:
    Memory();
    void call(const std::string& cmd);
    ~Memory();
};

Memory::Memory() {
    _mapping["load"] = &Memory::load;
}

void Memory::call(const std::string& cmd) {
    const std::string fct_name = cmd.substr(0, cmd.find(" "));
    robin_hood::unordered_map<std::string, shell_method_t>::iterator fct_iter = this->_mapping.find(fct_name);
    if (fct_iter != this->_mapping.end()) {
        (this->*(fct_iter->second))(cmd);
    } else {
        for (auto const& [key, val] : this->_filters) {
            this->do_query(key, val, cmd);
        }
    }
}

Memory::~Memory() {
}

void Memory::load(const std::string& cmd) {
    std::size_t first_space = cmd.find(" ");
    // TODO check if space exists
    const std::string& filename = cmd.substr(first_space + 1, cmd.length() - first_space - 1);
    std::cout << "loading " << filename << "..." << std::flush;
    // TODO test ecrase t on des données ?
    this->_filters[filename] = std::make_unique<fimpera<countingBF::CBF>>(filename);
    std::cout << " done." << std::endl;
}

}  // namespace shell

int main(int argc, char* argv[]) {
    argparse::ArgumentParser program("fimpera_shell", "0.0.1");
    parse(program, argc, argv);

    shell::Memory memory;
    std::string cmd;
    try {
        while ((cmd = shell::input()) != "exit") {
            memory.call(cmd);
        }
    } catch (const shell::eof& e) {
        return 0;
    }
}
