#pragma once

#include <string>

struct TreeParams {
    bool print_absolute = false;
    bool print_dirs_only = false;
    bool print_json = false;
    int indent_level = -1;
    std::string target;
};

void run_tree(const TreeParams &params);
