#include "tree.hpp"

#include "utils.hpp"

#include <filesystem>
#include <fmt/color.h>
#include <fmt/core.h>
#include <map>
#include <memory>
#include <optional>
#include <stdexcept>
#include <string>
#include <vector>

namespace fs = std::filesystem;

namespace {

enum FileType {
    REGULAR_FILE,
    DIRECTORY,
    OTHER,
};

struct FileNode {
    FileType filetype = REGULAR_FILE;
    std::optional<uintmax_t> filesize = std::nullopt;
    std::string filename;
    std::vector<std::unique_ptr<FileNode>> children;

    FileNode(const std::string &filename, const FileType filetype, const std::optional<uintmax_t> &filesize)
    {
        this->filename = filename;
        this->filetype = filetype;
        this->filesize = filesize;
    }
};

struct Stats {
    int max_depth = 0;
    int num_directories = 0;
    int num_files = 0;
    int num_other = 0;
    uintmax_t total_size = 0;
};

void print(int depth, const std::unique_ptr<FileNode> &node)
{
    static std::map<int, std::string> ws;

    if (!ws.contains(depth)) {
        ws.emplace(depth, std::string(depth * TAB_WIDTH, ' '));
    }

    switch (node->filetype) {
        case REGULAR_FILE:
            fmt::print("{}{} {}\n", ws[depth], node->filename, node->filesize.value());
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

void traverse_dir_layout(const std::unique_ptr<FileNode> &node, int depth = 0)
{
    print(depth, node);
    depth++;

    for (const auto &child: node->children) {
        traverse_dir_layout(child, depth);
    }
}

FileType inspect_entry(const fs::directory_entry &entry, Stats &stats, std::optional<uintmax_t> &size)
{
    if (entry.is_regular_file()) {
        size = fs::file_size(entry);

        stats.num_files++;
        stats.total_size += size.value();
        return REGULAR_FILE;
    }

    if (entry.is_directory()) {
        stats.num_directories++;
        return DIRECTORY;
    }

    stats.num_other++;
    return OTHER;
}

void precompute_dir_layout(const std::string &dir, FileNode &parent, Stats &stats, int depth = 0)
{
    stats.max_depth = ++depth;

    for (auto const &entry: fs::directory_iterator { dir }) {
        std::optional<uintmax_t> size = std::nullopt;
        FileType filetype = inspect_entry(entry, stats, size);

        std::string filename = entry.path().filename();
        std::unique_ptr<FileNode> child = std::make_unique<FileNode>(filename, filetype, size);

        if (filetype == DIRECTORY) {
            precompute_dir_layout(entry.path().string(), *child, stats, depth);
        }

        parent.children.push_back(std::move(child));
    }
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

    std::string target_s = target.string();
    strip_extra_path_delimiter(target_s);

    if (!fs::exists(target)) {
        throw std::runtime_error(fmt::format("Directory '{}' does not exist", target_s));
    }

    if (!fs::is_directory(target)) {
        throw std::runtime_error(fmt::format("'{}' is not a directory", target_s));
    }

    auto root = std::make_unique<FileNode>(target_s, DIRECTORY, std::nullopt);

    Stats stats;
    precompute_dir_layout(target_s, *root, stats);
    print_ruler(stats.max_depth);

    traverse_dir_layout(root);
    print_ruler_bottom(stats.max_depth);
    print_report(stats);
}
