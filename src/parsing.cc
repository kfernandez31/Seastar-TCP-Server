#include "parsing.hh"

#include <regex>
#include "validation.hh"

using std::string;
using std::regex;
const std::regex rgx_store("STORE\\$(.*)\\$(.*)\\$");
const std::regex rgx_load("LOAD\\$(.*)\\$");

string make_resp_found(const string& val) {
    string res(resp_found);
    res.insert(resp_found.length() - 1, val);
    return res;
}

op_type get_op_type_and_args(const string& req, string& key, string& val) {
    std::smatch matched_args;

    if (regex_search(req.begin(), req.end(), matched_args, rgx_store)) {
        key = matched_args[1];
        val = matched_args[2];
        return op_type::STORE;
    } else if (regex_search(req.begin(), req.end(), matched_args, rgx_load)) {
        key = matched_args[1];
        return op_type::LOAD;
    }

    return op_type::INVALID;
}