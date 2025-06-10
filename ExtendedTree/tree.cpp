#include "tree.hpp"

#include "filenode.hpp"
#include "params.hpp"
#include "reporting.hpp"
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

void discover_layout(const std::string &dir, filenode::FileNode &parent, Stats &stats)
{
    uintmax_t dir_size = 0;
    uintmax_t dir_disk_usage = utils::get_disk_usage(dir);
    uintmax_t num_children = 0;

    for (auto const &entry: fs::directory_iterator { dir }) {
        const std::string filename = entry.path().filename();
        std::unique_ptr<filenode::FileNode> child = std::make_unique<filenode::FileNode>(filename);

        if (entry.is_regular_file()) {
            child->set_is_file();
            uintmax_t size = fs::file_size(entry);
            uintmax_t disk_usage = utils::get_disk_usage(entry.path().string());

            child->set_filesize(size);
            child->set_disk_usage(disk_usage);
            dir_size += size;
            dir_disk_usage += disk_usage;
            stats.total_size += size;
            stats.num_files++;
            num_children++;
        } else if (entry.is_directory()) {
            child->set_is_directory();
            discover_layout(entry.path().string(), *child, stats);
            dir_size += child->get_filesize();
            dir_disk_usage += child->get_disk_usage(); // Accumulate children's disk usage
            num_children += child->get_num_children();
            stats.num_directories++;
        } else {
            child->set_is_other();
            stats.num_other++;
            num_children++;
        }

        parent.children.push_back(std::move(child));
    }

    if (parent.is_directory()) {
        parent.set_filesize(dir_size);
        parent.set_disk_usage(dir_disk_usage); // Set the cumulative disk usage
        parent.set_num_children(num_children);
    }
}

void print_pretty_output(const std::unique_ptr<filenode::FileNode> &root, const Stats &stats)
{
    if (params::PRINT_DIRS_ONLY) {
        reporting::print_pretty_output_dirs_only(root, stats.total_size);
    } else {
        reporting::print_pretty_output(root, stats.total_size);
    }

    fmt::print("\n");

    if (params::PRINT_HUMAN_READABLE) {
        fmt::print("Total size: {}\n", utils::bytes_to_human(stats.total_size));
    } else {
        fmt::print("Total size: {}\n", stats.total_size);
    }

    fmt::print("Number of directories: {}\n", stats.num_directories);
    fmt::print("Number of files: {}\n", stats.num_files);
    fmt::print("Number of other file-like objects: {}\n", stats.num_other);
}

} // namespace

namespace tree {

void run_tree()
{
    auto root = std::make_unique<filenode::FileNode>(params::TARGET);

    if (fs::is_directory(params::TARGET)) {
        root->set_is_directory();
    }

    Stats stats;
    discover_layout(params::TARGET, *root, stats);

    if (params::PRINT_JSON) {
        reporting::print_json(root, stats.total_size);
    } else {
        print_pretty_output(root, stats);
    }
}

} // namespace tree
