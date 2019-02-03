#include "config_parser_exception.h"

std::string ConfigParserException::what() {
    return "Invalid config.";
}