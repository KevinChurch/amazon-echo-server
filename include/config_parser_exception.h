#pragma once

#include <exception>
#include <string>

class ConfigParserException : public std::exception {
public:
    ConfigParserException() {}

    std::string what();
};