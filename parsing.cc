#include "validation.hh"

const std::string resp_done = "DONE$";
const std::string resp_not_found = "NOTFOUND$";
const std::string resp_found = "FOUND$$";
const std::regex rgx_store("STORE\\$(.*)\\$(.*)\\$");
const std::regex rgx_load("LOAD\\$(.*)\\$");

op_type get_op_type_and_args(const std::string& req, std::string& key, std::string& val) {
    std::smatch matched_args;

    if (std::regex_search(req.begin(), req.end(), matched_args, rgx_store)) {
        key = match[1];
        val = match[2];
        return op_type::STORE;
    } else if (std::regex_search(req.begin(), req.end(), matched_args, rgx_load)) {
        key = match[1];
        return op_type::LOAD;
    }

    return op_type::INVALID;
}