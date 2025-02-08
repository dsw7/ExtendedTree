#include "tree.hpp"

#include "filenode.hpp"
#include "params.hpp"
#include "reporting.hpp"
#include "utils.hpp"

#include <filesystem>
#include <fmt/core.h>
#include <json.hpp>
#include <memory>

namespace fs = std::filesystem;

namespace {

struct Stats {
    int max_depth = 0;
    int num_directories = 0;
    int num_files = 0;
    int num_other = 0;
    uintmax_t total_size = 0;
};

void precompute_dir_layout(const std::string &dir, FileNode &parent, Stats &stats, int depth = 0)
{
    stats.max_depth = ++depth;
    uintmax_t dir_size = 0;

    for (auto const &entry: fs::directory_iterator { dir }) {
        std::string filename = entry.path().filename();
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

void traverse_dirs_print_relative(const std::unique_ptr<FileNode> &node, uintmax_t total_size, int depth = 0)
{
    reporting::print(node, depth, total_size);
    depth++;

    for (const auto &child: node->children) {
        traverse_dirs_print_relative(child, total_size, depth);
    }
}

void traverse_dirs_print_absolute(const std::unique_ptr<FileNode> &node, int depth = 0)
{
    reporting::print(node, depth);
    depth++;

    for (const auto &child: node->children) {
        traverse_dirs_print_absolute(child, depth);
    }
}

void traverse_dirs_print_relative_dirs(const std::unique_ptr<FileNode> &node, uintmax_t total_size, int depth = 0)
{
    reporting::print_dirs_only(node, depth, total_size);
    depth++;

    for (const auto &child: node->children) {
        traverse_dirs_print_relative_dirs(child, total_size, depth);
    }
}

void traverse_dirs_print_absolute_dirs(const std::unique_ptr<FileNode> &node, int depth = 0)
{
    reporting::print_dirs_only(node, depth);
    depth++;

    for (const auto &child: node->children) {
        traverse_dirs_print_absolute_dirs(child, depth);
    }
}

nlohmann::json traverse_dirs_build_json(const std::unique_ptr<FileNode> &node)
{
    nlohmann::json j;

    j["filename"] = node->filename;

    if (node->is_file()) {
        j["filesize"] = node->get_filesize();
    } else if (node->is_directory()) {
        j["filesize"] = node->get_filesize();
    } else {
        j["filesize"] = nullptr;
    }

    j["children"] = nlohmann::json::array();

    for (const auto &child: node->children) {
        j["children"].push_back(traverse_dirs_build_json(child));
    }

    return j;
}

nlohmann::json traverse_dirs_build_json(const std::unique_ptr<FileNode> &node, uintmax_t total_size)
{
    nlohmann::json j;

    j["filename"] = node->filename;

    if (node->is_file()) {
        j["filesize"] = compute_relative_usage(node->get_filesize(), total_size);
    } else if (node->is_directory()) {
        j["filesize"] = compute_relative_usage(node->get_filesize(), total_size);
    } else {
        j["filesize"] = nullptr;
    }

    j["children"] = nlohmann::json::array();

    for (const auto &child: node->children) {
        j["children"].push_back(traverse_dirs_build_json(child, total_size));
    }

    return j;
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
        nlohmann::json json;

        if (params.print_absolute) {
            json = traverse_dirs_build_json(root);
        } else {
            json = traverse_dirs_build_json(root, stats.total_size);
        }

        fmt::print("{}\n", json.dump(params.indent_level));
        return;
    }

    if (params.print_absolute && params.print_dirs_only) {
        traverse_dirs_print_absolute_dirs(root);
    } else if (params.print_absolute && !params.print_dirs_only) {
        traverse_dirs_print_absolute(root);
    } else if (!params.print_absolute && params.print_dirs_only) {
        traverse_dirs_print_relative_dirs(root, stats.total_size);
    } else {
        traverse_dirs_print_relative(root, stats.total_size);
    }

    fmt::print("\n");
    fmt::print("Total size: {} bytes\n", stats.total_size);
    fmt::print("Number of directories: {}\n", stats.num_directories);
    fmt::print("Number of files: {}\n", stats.num_files);
    fmt::print("Number of other file-like objects: {}\n", stats.num_other);
}
