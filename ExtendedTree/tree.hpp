#pragma once

#include <filesystem>
#include <optional>

struct TreeParams {
    bool print_absolute = false;
    bool print_dirs_only = false;
    std::optional<std::filesystem::path> target = std::nullopt;
};

void run_tree(const TreeParams &params);
