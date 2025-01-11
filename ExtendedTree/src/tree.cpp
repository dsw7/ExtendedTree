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

struct FileStats {
    int num_directories = 0;
    int num_files = 0;
    int num_other = 0;
};

void process_file(const fs::recursive_directory_iterator &file, FileStats &stats)
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
        stats.num_directories++;
    } else if (file->is_regular_file()) {
        fmt::print("{}{}\n", ws[depth], filename);
        stats.num_files++;
    } else {
        stats.num_other++;
    }
}

void iterate_over_dirs(const fs::path &target)
{
    FileStats fs;

    for (auto it = fs::recursive_directory_iterator(target); it != fs::recursive_directory_iterator(); ++it) {
        process_file(it, fs);
    }

    fmt::print("\nNumber of directories: {}\n", fs.num_directories);
    fmt::print("Number of files: {}\n", fs.num_files);
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
