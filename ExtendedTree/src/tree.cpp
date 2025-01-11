#include "tree.hpp"

#include <filesystem>
#include <fmt/core.h>
#include <iostream>
#include <stdexcept>

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
        throw std::runtime_error(fmt::format("Directory '{}' does not exist", target.string()));
    }

    if (!fs::is_directory(target)) {
        throw std::runtime_error(fmt::format("'{}' is not a directory", target.string()));
    }

    for (auto it = fs::recursive_directory_iterator(target); it != fs::recursive_directory_iterator(); ++it) {
        std::cout << it.depth() << ' ' << *it << '\n';
    }
}
