#include "tree.hpp"

#include "filenode.hpp"
#include "utils.hpp"

#include <filesystem>
#include <fmt/core.h>
#include <memory>
#include <optional>

namespace fs = std::filesystem;

namespace {

struct Stats {
    int max_depth = 0;
    int num_directories = 0;
    int num_files = 0;
    int num_other = 0;
    uintmax_t total_size = 0;
};

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
    uintmax_t dir_size = 0;

    for (auto const &entry: fs::directory_iterator { dir }) {
        std::optional<uintmax_t> size = std::nullopt;
        FileType filetype = inspect_entry(entry, stats, size);

        std::string filename = entry.path().filename();
        std::unique_ptr<FileNode> child = std::make_unique<FileNode>(filename, filetype, size);

        if (filetype == DIRECTORY) {
            precompute_dir_layout(entry.path().string(), *child, stats, depth);

            if (child->filesize.has_value()) {
                dir_size += child->filesize.value();
            }
        } else if (filetype == REGULAR_FILE && size.has_value()) {
            dir_size += size.value();
        }

        parent.children.push_back(std::move(child));
    }

    if (parent.filetype == DIRECTORY) {
        parent.filesize = dir_size;
    }
}

void traverse_dirs_print_relative(const std::unique_ptr<FileNode> &node, int total_size, int depth = 0)
{
    node->print(depth, total_size);
    depth++;

    for (const auto &child: node->children) {
        traverse_dirs_print_relative(child, total_size, depth);
    }
}

void traverse_dirs_print_absolute(const std::unique_ptr<FileNode> &node, int depth = 0)
{
    node->print(depth);
    depth++;

    for (const auto &child: node->children) {
        traverse_dirs_print_absolute(child, depth);
    }
}

void traverse_dirs_print_relative_dirs(const std::unique_ptr<FileNode> &node, int total_size, int depth = 0)
{
    node->print_dirs_only(depth, total_size);
    depth++;

    for (const auto &child: node->children) {
        traverse_dirs_print_relative_dirs(child, total_size, depth);
    }
}

void traverse_dirs_print_absolute_dirs(const std::unique_ptr<FileNode> &node, int depth = 0)
{
    node->print_dirs_only(depth);
    depth++;

    for (const auto &child: node->children) {
        traverse_dirs_print_absolute_dirs(child, depth);
    }
}

void print_report(const Stats &stats)
{
    fmt::print("\n");
    fmt::print("Total size: {} bytes\n", stats.total_size);
    fmt::print("Number of directories: {}\n", stats.num_directories);
    fmt::print("Number of files: {}\n", stats.num_files);
    fmt::print("Number of other file-like objects: {}\n", stats.num_other);
}

} // namespace

void run_tree(const TreeParams &params)
{
    auto root = std::make_unique<FileNode>(params.target, DIRECTORY, std::nullopt);

    Stats stats;
    precompute_dir_layout(params.target, *root, stats);

    print_ruler(stats.max_depth, true);

    if (params.print_absolute) {
        if (params.print_dirs_only) {
            traverse_dirs_print_absolute_dirs(root);
        } else {
            traverse_dirs_print_absolute(root);
        }
    } else {
        if (params.print_dirs_only) {
            traverse_dirs_print_relative_dirs(root, stats.total_size);
        } else {
            traverse_dirs_print_relative(root, stats.total_size);
        }
    }

    print_ruler(stats.max_depth, false);

    print_report(stats);
}
