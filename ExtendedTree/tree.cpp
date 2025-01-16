#include "tree.hpp"

#include <filesystem>
#include <fmt/color.h>
#include <fmt/core.h>
#include <iostream>
#include <map>
#include <stdexcept>
#include <string>
#include <vector>

namespace fs = std::filesystem;

namespace {

const std::string elbow = "└";
const std::string hline = "─";
const std::string tee = "├";
const std::string vline = "│";

std::map<int, std::string> ws = {};

struct Stats {
    uintmax_t size = 0;
};

void insert_new_ws(int depth)
{
    if (!ws.contains(depth)) {
        ws.emplace(depth, std::string(depth * 4, ' '));
    }
}

void print_dir(int depth, const fs::directory_entry &dir, uintmax_t dir_size)
{
    insert_new_ws(depth);
    std::cout << ws[depth] << dir.path().filename() << "/ " << dir_size << '\n';
}

uintmax_t print_file(int depth, const fs::directory_entry &file)
{
    insert_new_ws(depth);
    uintmax_t size = fs::file_size(file);

    std::cout << ws[depth] << file.path().filename() << ' ' << size << '\n';
    return size;
}

uintmax_t iterate_over_dirs(const std::string &dir, Stats &s, int depth = 0)
{
    uintmax_t dir_total_size = 0;

    for (auto const &dir_entry: fs::directory_iterator { dir }) {

        if (dir_entry.is_directory()) {
            uintmax_t dir_size = iterate_over_dirs(dir_entry.path().string(), s, depth + 1);
            print_dir(depth, dir_entry, dir_size);
            dir_total_size += dir_size;
        }

        if (dir_entry.is_regular_file()) {
            uintmax_t file_size = print_file(depth, dir_entry);
            dir_total_size += file_size;
            s.size += file_size;
        }
    }

    return dir_total_size;
}

void print_report(const Stats &stats)
{
    fmt::print("Total size: {}\n", stats.size);
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

    fmt::print(fg(fmt::terminal_color::bright_blue), "{}/\n", target.string());

    Stats stats;
    iterate_over_dirs(target, stats);

    print_report(stats);
}
