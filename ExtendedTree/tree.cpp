#include "tree.hpp"

#include "filenode.hpp"
#include "params.hpp"
#include "report_json.hpp"
#include "report_pretty.hpp"
#include "utils.hpp"

#include <filesystem>
#include <fmt/core.h>
#include <memory>

namespace fs = std::filesystem;

namespace {

struct Stats {
    int num_directories = 0;
    int num_files = 0;
    int num_other = 0;
    uintmax_t total_size = 0;
};

void precompute_dir_layout(const std::string &dir, FileNode &parent, Stats &stats, int depth = 0)
{
    depth++;
    uintmax_t dir_size = 0;

    for (auto const &entry: fs::directory_iterator { dir }) {
        std::string filename = entry.path().filename();

        if (params.excludes.contains(filename)) {
            continue;
        }

        std::unique_ptr<FileNode> child = std::make_unique<FileNode>(filename);

        if (entry.is_regular_file()) {
            child->set_is_file();
            uintmax_t size = fs::file_size(entry);
            child->set_filesize(size);
            stats.total_size += size;
            stats.num_files++;
        } else if (entry.is_directory()) {
            child->set_is_directory();
            stats.num_directories++;
        } else {
            child->set_is_other();
            stats.num_other++;
        }

        if (child->is_directory()) {
            precompute_dir_layout(entry.path().string(), *child, stats, depth);
            dir_size += child->get_filesize();
        } else if (child->is_file()) {
            dir_size += child->get_filesize();
        }

        parent.children.push_back(std::move(child));
    }

    if (parent.is_directory()) {
        parent.set_filesize(dir_size);
    }
}

} // namespace

void run_tree()
{
    auto root = std::make_unique<FileNode>(params.target);

    if (fs::is_directory(params.target)) {
        root->set_is_directory();
    }

    Stats stats;
    precompute_dir_layout(params.target, *root, stats);

    if (params.print_json) {
        if (params.print_absolute) {
            reporting::print_json(root);
        } else {
            reporting::print_json(root, stats.total_size);
        }
        return;
    }

    if (params.print_absolute && params.print_dirs_only) {
        reporting::print_absolute_dirs(root);
    } else if (params.print_absolute && !params.print_dirs_only) {
        reporting::print_absolute(root);
    } else if (!params.print_absolute && params.print_dirs_only) {
        reporting::print_relative_dirs(root, stats.total_size);
    } else {
        reporting::print_relative(root, stats.total_size);
    }

    fmt::print("\n");

    if (params.print_bytes) {
        fmt::print("Total size: {}\n", stats.total_size);
    } else {
        fmt::print("Total size: {}\n", utils::bytes_to_human(stats.total_size));
    }

    fmt::print("Number of directories: {}\n", stats.num_directories);
    fmt::print("Number of files: {}\n", stats.num_files);
    fmt::print("Number of other file-like objects: {}\n", stats.num_other);
}
