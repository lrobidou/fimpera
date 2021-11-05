#pragma once

#include <cxxopts.hpp>

template <typename T>
T getOneArg(const cxxopts::ParseResult& arguments, const std::string& argName) {
    try {
        return arguments[argName].as<T>();
    } catch (const std::exception& e) {
        std::cerr << "It appears you did not provide a valid parameter for \"" << argName << "\"" << std::endl;
        std::cerr << "Worry not, for option -h displays some help." << std::endl;
        exit(-1);
    }
}

template <typename T>
T getOneArgOptional(const cxxopts::ParseResult& arguments, const std::string& argName, const T& defaultvalue) {
    try {
        return arguments[argName].as<T>();
    } catch (const std::exception& e) {
        std::cout << "using default value for " << argName << " = " << defaultvalue << std::endl;  // TODO remove ?
        return defaultvalue;
    }
}