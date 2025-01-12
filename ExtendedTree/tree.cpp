#include "tree.hpp"

#include <filesystem>
#include <fmt/color.h>
#include <fmt/core.h>
#include <map>
#include <stdexcept>
#include <string>
#include <vector>

namespace fs = std::filesystem;

namespace shapes {
const std::string elbow = "└";
const std::string hline = "─";
const std::string vline = "│";
} // namespace shapes

namespace {

struct FileStats {
    int num_directories = 0;
    int num_files = 0;
    int num_other = 0;
};

void process_file(const fs::recursive_directory_iterator &file)
{
    int depth = file.depth() + 1;

    static std::map<int, std::string> ws = {};
    static int tw = 4;

    if (!ws.contains(depth)) {
        ws[depth] = std::string(depth * tw, ' ');
    }

    const std::string filename = file->path().filename();

    if (file->is_directory()) {
        fmt::print(fg(fmt::terminal_color::bright_blue), "{}{}/\n", ws[depth], filename);
    } else if (file->is_regular_file()) {
        fmt::print("{}{}\n", ws[depth], filename);
    }
}

struct State {
    bool is_directory = false;
    bool is_file = false;
    bool is_other = false;

    int depth_delta = -100;
    int depth = 0;

    std::string filename;
};

void map_directory_structure(const fs::recursive_directory_iterator &file, State &s)
{
    s.depth = file.depth() + 1;
    s.filename = file->path().filename();

    if (file->is_directory()) {
        s.is_directory = true;
    } else if (file->is_regular_file()) {
        s.is_file = true;
    } else {
        s.is_other = true;
    }
}

void iterate_over_dirs(const fs::path &target, std::vector<State> &rows)
{
    fmt::print(fg(fmt::terminal_color::bright_blue), "{}/\n", target.string());

    for (auto it = fs::recursive_directory_iterator(target); it != fs::recursive_directory_iterator(); ++it) {
        process_file(it);

        State state;
        map_directory_structure(it, state);
        rows.push_back(state);
    }
}

void compute_change_in_depth(std::vector<State> &rows)
{
    int num_rows = rows.size();

    for (int i = 0; i < num_rows; i++) {
        if (i > 0) {
            rows[i].depth_delta = rows[i].depth - rows[i - 1].depth;
        }
    }
}

void process_rows(const std::vector<State> &rows)
{
    int size = rows.size() - 1;

    for (int i = 0; i <= size; i++) {
        fmt::print("{} {} {}\n", rows[i].filename, rows[i].depth, rows[i].depth_delta);
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

    std::vector<State> rows;
    iterate_over_dirs(target, rows);
    compute_change_in_depth(rows);

    fmt::print(fg(fmt::terminal_color::bright_blue), "{}/\n", target.string());
    process_rows(rows);
}
