#include "tree.hpp"

#include <filesystem>
#include <fmt/core.h>
#include <iostream>

namespace fs = std::filesystem;

void run_tree(const Params &params)
{
    fs::path target;

    if (params.target.has_value()) {
        target = params.target.value();
    } else {
        target = fs::current_path();
    }

    if (!fs::exists(target)) {
        fmt::print(stderr, "Directory '{}' does not exist\n", target.string());
        return;
    }

    if (!fs::is_directory(target)) {
        fmt::print(stderr, "'{}' is not a directory\n", target.string());
        return;
    }

    for (auto it = fs::recursive_directory_iterator(target); it != fs::recursive_directory_iterator(); ++it) {
        std::cout << it.depth() << ' ' << *it << '\n';
    }
}
