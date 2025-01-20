#pragma once

#include <string>

struct TreeParams {
    bool print_absolute = false;
    bool print_dirs_only = false;
    std::string target;
};

void run_tree(const TreeParams &params);
