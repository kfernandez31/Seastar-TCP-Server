#include "parsing.hh"
#include "validation.hh"
#include <regex>

using std::string;
using std::regex;

const std::regex rgx_store("STORE\\$(.*)\\$(.*)\\$");
const std::regex rgx_load("LOAD\\$(.*)\\$");

string make_resp_found(const string& val) {
    string res(resp_found);
    res.insert(resp_found.length() - 1, val);
    return res;
}

op_type get_op_type(const string& req, std::smatch& matched_args) {
    if (regex_search(req.begin(), req.end(), matched_args, rgx_store)) 
        return op_type::STORE;
    if (regex_search(req.begin(), req.end(), matched_args, rgx_load)) 
        return op_type::LOAD;
    return op_type::INVALID;
}
