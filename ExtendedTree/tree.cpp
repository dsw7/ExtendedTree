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

std::map<int, std::string> ws;

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
    fmt::print(fg(fmt::terminal_color::bright_blue), "{}{}/ ", ws[depth], dir.path().filename().string());
    fmt::print(fg(fmt::terminal_color::bright_green), "{}\n", dir_size);
}

void print_file(int depth, const fs::directory_entry &file, uintmax_t file_size)
{
    fmt::print("{}{} {}\n", ws[depth], file.path().filename().string(), file_size);
}

uintmax_t iterate_over_dirs(const std::string &dir, Stats &stats, int depth)
{
    insert_new_ws(depth);

    uintmax_t dir_total_size = 0;

    for (auto const &entry: fs::directory_iterator { dir }) {

        if (entry.is_directory()) {
            stats.num_directories++;
            uintmax_t dir_size = iterate_over_dirs(entry.path().string(), stats, depth + 1);
            print_dir(depth, entry, dir_size);
            dir_total_size += dir_size;
        } else if (entry.is_regular_file()) {
            stats.num_files++;
            uintmax_t file_size = fs::file_size(entry);
            print_file(depth, entry, file_size);
            dir_total_size += file_size;
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
    int start_depth = 1;
    uintmax_t total_size = iterate_over_dirs(target, stats, start_depth);
    stats.total_size = total_size;

    print_report(stats);
}
