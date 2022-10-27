#pragma once

#include <string>

typedef enum op_type {
    INVALID,
    STORE,
    LOAD,
} op_type;

op_type get_op_type_and_args(const std::string& req, std::string& key, std::string& val);
