#include "parsing.hh"

std::string make_resp_found(const std::string& val) {
    std::string res(resp_found);
    res.insert(resp_found.length() - 1, val);
    return res;
}

cmd_type get_cmd_type(const std::string& req, std::smatch& matched_args) {
    if (regex_search(req.begin(), req.end(), matched_args, rgx_store)) 
        return cmd_type::STORE;
    if (regex_search(req.begin(), req.end(), matched_args, rgx_load)) 
        return cmd_type::LOAD;
    return cmd_type::INVALID;
}
