#pragma once

typedef enum op_type {
    INVALID
    STORE,
    LOAD,
} req_type;

op_type get_op_type_and_args(const std::string& req, std::string& key, std::string& val);
