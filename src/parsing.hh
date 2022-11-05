#pragma once

#include <regex>
#include <string>

const std::string resp_unknown_cmd = "UNKNOWN_COMMAND$";
const std::string resp_invalid_args = "INVALID_ARGS$";
const std::string resp_done = "DONE$";
const std::string resp_not_found = "NOTFOUND$";
const std::string resp_found = "FOUND$$";
const std::regex rgx_store("STORE\\$([a-z]*)\\$([a-z]*)\\$");
const std::regex rgx_load("LOAD\\$([a-z]*)\\$");

typedef enum cmd_type {
    INVALID,
    STORE,
    LOAD,
} cmd_type;

std::string make_resp_found(const std::string& val);

cmd_type get_cmd_type(const std::string& req, std::smatch& matched_args);
