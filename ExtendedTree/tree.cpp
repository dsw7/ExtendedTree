#include "tree.hpp"

#include "filenode.hpp"
#include "utils.hpp"

#include <filesystem>
#include <fmt/core.h>
#include <memory>
#include <optional>
#include <stdexcept>
#include <string>

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

void traverse_dir_layout(const std::unique_ptr<FileNode> &node, int depth = 0)
{
    node->print(depth);
    depth++;

    for (const auto &child: node->children) {
        traverse_dir_layout(child, depth);
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

void run_tree(const TreeParams &params)
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

    print_ruler(stats.max_depth, true);
    traverse_dir_layout(root);
    print_ruler(stats.max_depth, false);

    print_report(stats);
}
