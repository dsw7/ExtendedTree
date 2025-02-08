#pragma once

#include <set>
#include <string>

struct TreeParams {
    bool print_absolute = false;
    bool print_dirs_only = false;
    bool print_json = false;
    int indent_level = -1;
    std::set<std::string> excludes;
    std::string target;
};

extern struct TreeParams params;
