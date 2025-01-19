#pragma once

#include <filesystem>
#include <optional>

struct TreeParams {
    std::optional<std::filesystem::path> target = std::nullopt;
};

void run_tree(const TreeParams &params);
