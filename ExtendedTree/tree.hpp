#pragma once

#include <filesystem>
#include <optional>

struct TreeParams {
    bool print_absolute = false;
    std::optional<std::filesystem::path> target = std::nullopt;
};

void run_tree(const TreeParams &params);
