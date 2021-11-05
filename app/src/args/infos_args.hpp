#pragma once

#include "args.hpp"

cxxopts::ParseResult parseArgvInfo(int argc, char* argv[]) {
    cxxopts::Options options("index", "indexer of multifilter");
    options.add_options()                                                                                         //
        ("i,input", "name of the index file", cxxopts::value<std::string>())                                      //
        ("h,help", "display this help", cxxopts::value<bool>()->default_value("false")->implicit_value("true"));  //
    return options.parse(argc, argv);
}

std::tuple<std::string> getArgsInfo(const cxxopts::ParseResult& arguments) {
    const bool displayHelp = getOneArg<bool>(arguments, "h");
    if (displayHelp) {
        std::cout << "fimpera_info: retrieve informations stored in a fimpera index." << std::endl;
        std::cout << "Usage:" << std::endl;
        std::cout << "    ./bin/fimpera_info -i inputFile" << std::endl;
        std::cout << "Options:" << std::endl;
        std::cout << "    -i: the index you want informations about." << std::endl;
        std::cout << "Example:" << std::endl;
        std::cout << "    ./bin/fimpera_info -i index.idx" << std::endl;
        exit(0);
    }

    // mandatory args
    const std::string input_filename = getOneArg<std::string>(arguments, "i");

    return {input_filename};
}