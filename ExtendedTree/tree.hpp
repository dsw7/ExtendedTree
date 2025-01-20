#pragma once

#include <filesystem>

struct TreeParams {
    bool print_absolute = false;
    bool print_dirs_only = false;
    std::filesystem::path target;
};

void run_tree(const TreeParams &params);
