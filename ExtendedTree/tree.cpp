#include "tree.hpp"

#include <filesystem>
#include <fmt/color.h>
#include <fmt/core.h>
#include <map>
#include <stdexcept>
#include <string>
#include <vector>

namespace fs = std::filesystem;

namespace {

struct FileStats {
    int num_directories = 0;
    int num_files = 0;
    int num_other = 0;
};

struct Line {
    bool is_directory = false;
    bool is_file = false;
    bool is_other = false;

    int depth_delta = 1;
    int depth = 0;

    std::string filename;
};

void map_directory_structure(const fs::recursive_directory_iterator &file, Line &line)
{
    line.depth = file.depth();
    line.filename = file->path().filename();

    if (file->is_directory()) {
        line.is_directory = true;
    } else if (file->is_regular_file()) {
        line.is_file = true;
    } else {
        line.is_other = true;
    }
}

void iterate_over_dirs(const fs::path &target, std::vector<Line> &lines)
{
    for (auto it = fs::recursive_directory_iterator(target); it != fs::recursive_directory_iterator(); ++it) {
        Line line;
        map_directory_structure(it, line);
        lines.push_back(line);
    }
}

void compute_change_in_depth(std::vector<Line> &lines)
{
    int num_lines = lines.size();

    for (int i = 0; i < num_lines; i++) {
        if (i > 0) {
            lines[i].depth_delta = lines[i].depth - lines[i - 1].depth;
        }
    }
}

std::string get_prefix(int width)
{
    static std::string elbow = "└";
    static std::string hline = "─";
    static std::string tee = "├";
    static std::string vline = "│";

    std::string prefix;

    for (int i = 0; i < width - 1; i++) {
        if (i == 0) {
            prefix += tee;
            continue;
        }

        prefix += hline;
    }

    prefix += ' ';
    return prefix;
}

void process_line(const Line &line)
{
    int depth = line.depth + 1;

    static std::map<int, std::string> ws = {};
    static int tw = 4;

    if (!ws.contains(depth)) {
        ws[depth] = get_prefix(depth * tw);
    }

    fmt::print("{}", ws[depth]);

    if (line.is_directory) {
        fmt::print(fg(fmt::terminal_color::bright_blue), "{}/ {}\n", line.filename, line.depth_delta);
    } else if (line.is_file) {
        fmt::print("{} {}\n", line.filename, line.depth_delta);
    }
}

void process_lines(const std::vector<Line> &lines)
{
    int size = lines.size() - 1;

    for (int i = 0; i <= size; i++) {
        process_line(lines[i]);
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

    std::vector<Line> lines;
    iterate_over_dirs(target, lines);
    compute_change_in_depth(lines);

    fmt::print(fg(fmt::terminal_color::bright_blue), "{}/\n", target.string());
    process_lines(lines);
}
