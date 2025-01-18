#include "tree.hpp"

#include <filesystem>
#include <fmt/color.h>
#include <fmt/core.h>
#include <map>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

namespace fs = std::filesystem;

namespace {

const std::string elbow = "└";
const std::string hline = "─";
const std::string tee = "├";
const std::string vline = "│";

enum FileType {
    REGULAR_FILE,
    DIRECTORY,
    OTHER,
};

struct FileNode {
    FileType filetype = REGULAR_FILE;
    std::string filename;
    std::vector<std::unique_ptr<FileNode>> children;

    FileNode(const std::string &filename, const FileType filetype)
    {
        this->filename = filename;
        this->filetype = filetype;
    }
};

void print(int depth, const std::unique_ptr<FileNode> &node)
{
    static std::map<int, std::string> ws;

    if (!ws.contains(depth)) {
        ws.emplace(depth, std::string(depth * 4, ' '));
    }

    switch (node->filetype) {
        case REGULAR_FILE:
            fmt::print("{}{}\n", ws[depth], node->filename);
            break;
        case DIRECTORY:
            fmt::print(fg(fmt::terminal_color::bright_blue), "{}{}/\n", ws[depth], node->filename);
            break;
        case OTHER:
            fmt::print(fg(fmt::terminal_color::bright_green), "{}{} ?\n", ws[depth], node->filename);
            break;
        default:
            fmt::print(fg(fmt::terminal_color::bright_green), "{}{} ?\n", ws[depth], node->filename);
    }
}

void traverse(const std::unique_ptr<FileNode> &node, int depth = 0)
{
    print(depth, node);
    depth++;

    for (const auto &child: node->children) {
        traverse(child, depth);
    }
}

struct Stats {
    int num_directories = 0;
    int num_files = 0;
    int num_other = 0;
    uintmax_t total_size = 0;
};

void iterate_over_dirs(const std::string &dir, FileNode &parent)
{
    for (auto const &entry: fs::directory_iterator { dir }) {
        FileType filetype;

        if (entry.is_regular_file()) {
            filetype = REGULAR_FILE;
        } else if (entry.is_directory()) {
            filetype = DIRECTORY;
        } else {
            filetype = OTHER;
        }

        std::string filename = entry.path().filename();
        std::unique_ptr<FileNode> child = std::make_unique<FileNode>(filename, filetype);

        if (filetype == DIRECTORY) {
            iterate_over_dirs(entry.path().string(), *child);
        }

        parent.children.push_back(std::move(child));
    }
}

/*
void print_report(const Stats &stats)
{
    fmt::print("\nTotal size: {} bytes\n", stats.total_size);
    fmt::print("Number of directories: {}\n", stats.num_directories);
    fmt::print("Number of files: {}\n", stats.num_files);
    fmt::print("Number of other file-like objects: {}\n", stats.num_other);
}
*/

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

    auto root = std::make_unique<FileNode>(target.string(), DIRECTORY);
    iterate_over_dirs(target.string(), *root);
    traverse(root);

    /*
    Stats stats;
    int start_depth = 1;
    uintmax_t total_size = iterate_over_dirs(target, stats, start_depth);
    stats.total_size = total_size;
    */

    // print_report(stats);
}
