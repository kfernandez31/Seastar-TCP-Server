#include "validation.hh"
#include <regex>

bool is_valid(const std::string& str) {
    return std::regex_match(str, std::regex("[a-z]*"));
}
