#pragma once

#include <string>
#include <optional>

const std::string resp_invalid_op = "INVALID_OPERATION$";
const std::string resp_done = "DONE$";
const std::string resp_not_found = "NOTFOUND$";
const std::string resp_found = "FOUND$$";

typedef enum op_type {
    INVALID,
    STORE,
    LOAD,
} op_type;

std::string make_resp_found(const std::string& val);

op_type get_op_type_and_args(const std::string& req, std::string& key, std::string& val);