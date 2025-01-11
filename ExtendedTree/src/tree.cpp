#include "tree.hpp"

#include <filesystem>
#include <fmt/core.h>
#include <iostream>
#include <map>
#include <stdexcept>
#include <string>

namespace fs = std::filesystem;

namespace {

void iterate_over_dirs(const fs::path &target)
{
    static std::map<int, std::string> ws = {};
    static int tw = 4;
    int depth = 0;

    for (auto it = fs::recursive_directory_iterator(target); it != fs::recursive_directory_iterator(); ++it) {
        depth = it.depth();

        if (!ws.contains(depth)) {
            ws[depth] = std::string(depth * tw, ' ');
        }

        std::string t = it->path();
        std::cout << ws[depth] << t << '\n';
    }
}

} // namespace

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

    iterate_over_dirs(target);
}
