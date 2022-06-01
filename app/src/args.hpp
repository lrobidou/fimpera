#pragma once
#include <argparse/argparse.hpp>

void parse(argparse::ArgumentParser& program, int argc, char* argv[]) {
    try {
        program.parse_args(argc, argv);
    } catch (const std::runtime_error& err) {
        std::cerr << err.what() << std::endl;
        std::cerr << program;
        std::exit(1);
    }
}