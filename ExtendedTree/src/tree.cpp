#include "tree.hpp"

#include <filesystem>
#include <fmt/color.h>
#include <fmt/core.h>
#include <iostream>
#include <map>
#include <stdexcept>
#include <string>

namespace fs = std::filesystem;

namespace {

void process_file(const fs::recursive_directory_iterator &file)
{
    int depth = file.depth();

    static std::map<int, std::string> ws = {};
    static int tw = 4;

    if (!ws.contains(depth)) {
        ws[depth] = std::string(depth * tw, ' ');
    }

    std::string filename = file->path().filename();

    if (file->is_directory()) {
        fmt::print(fg(fmt::terminal_color::bright_blue), "{}{}/\n", ws[depth], filename);
    } else {
        std::cout << ws[depth] << filename << '\n';
    }
}

void iterate_over_dirs(const fs::path &target)
{
    for (auto it = fs::recursive_directory_iterator(target); it != fs::recursive_directory_iterator(); ++it) {
        process_file(it);
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
