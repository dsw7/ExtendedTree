#include "tree.hpp"

#include <filesystem>
#include <fmt/color.h>
#include <fmt/core.h>
#include <map>
#include <stdexcept>
#include <string>

namespace fs = std::filesystem;

namespace {

const std::string elbow = "└";
const std::string hline = "─";
const std::string tee = "├";
const std::string vline = "│";

std::map<int, std::string> ws = {};

struct Stats {
    int num_directories = 0;
    int num_files = 0;
    int num_other = 0;
    uintmax_t total_size = 0;
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
    fmt::print(fg(fmt::terminal_color::bright_blue), "{}{}/ ", ws[depth], dir.path().filename().string());
    fmt::print(fg(fmt::terminal_color::bright_green), "{}\n", dir_size);
}

uintmax_t print_file(int depth, const fs::directory_entry &file)
{
    insert_new_ws(depth);
    uintmax_t size = fs::file_size(file);

    fmt::print("{}{} {}\n", ws[depth], file.path().filename().string(), size);
    return size;
}

uintmax_t iterate_over_dirs(const std::string &dir, Stats &stats, int depth = 0)
{
    uintmax_t dir_total_size = 0;

    for (auto const &entry: fs::directory_iterator { dir }) {

        if (entry.is_directory()) {
            uintmax_t dir_size = iterate_over_dirs(entry.path().string(), stats, depth + 1);
            print_dir(depth, entry, dir_size);
            dir_total_size += dir_size;
            stats.num_directories++;
        } else if (entry.is_regular_file()) {
            uintmax_t file_size = print_file(depth, entry);
            dir_total_size += file_size;
            stats.num_files++;
        } else {
            stats.num_other++;
        }
    }

    return dir_total_size;
}

void print_report(const Stats &stats)
{
    fmt::print("\nTotal size: {} bytes\n", stats.total_size);
    fmt::print("Number of directories: {}\n", stats.num_directories);
    fmt::print("Number of files: {}\n", stats.num_files);
    fmt::print("Number of other file-like objects: {}\n", stats.num_other);
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
    uintmax_t total_size = iterate_over_dirs(target, stats);
    stats.total_size = total_size;

    print_report(stats);
}
