#include "reporting.hpp"

#include "params.hpp"
#include "utils.hpp"

#include <fmt/color.h>
#include <fmt/core.h>
#include <json.hpp>
#include <string>

namespace {

constexpr fmt::terminal_color blue = fmt::terminal_color::bright_blue;
constexpr fmt::terminal_color cyan = fmt::terminal_color::bright_cyan;
constexpr fmt::terminal_color green = fmt::terminal_color::bright_green;

bool skip_level(int depth)
{
    if (params::LEVEL < 1) {
        return false;
    }

    if (params::LEVEL == depth - 1) {
        return true;
    }

    return false;
}

void append_usage(std::string &line, uintmax_t size, uintmax_t total_size)
{
    float relative_size = utils::compute_relative_usage(size, total_size);

    if (params::PRINT_HUMAN_READABLE) {
        line += fmt::format(fg(green), "[ {}, {:.{}f}% ]", utils::bytes_to_human(size), relative_size, 2);
    } else {
        line += fmt::format(fg(green), "[ {} bytes, {:.{}f}% ]", size, relative_size, 2);
    }
}

void print_file(const std::unique_ptr<filenode::FileNode> &node, uintmax_t total_size)
{
    std::string line = fmt::format("{} ", node->filename);
    append_usage(line, node->get_filesize(), total_size);
    fmt::print("{}\n", line);
}

void print_directory(const std::unique_ptr<filenode::FileNode> &node, uintmax_t total_size)
{
    std::string line = fmt::format(fg(blue), "{}/ ", node->filename);
    append_usage(line, node->get_filesize(), total_size);
    fmt::print("{}\n", line);
}

void print_other(const std::string &filename)
{
    fmt::print(fg(cyan), "{} ?\n", filename);
}

void print_dirs_files_or_other(const std::unique_ptr<filenode::FileNode> &node, uintmax_t total_size)
{
    if (node->is_file()) {
        print_file(node, total_size);
    } else if (node->is_directory()) {
        print_directory(node, total_size);
    } else {
        print_other(node->filename);
    }
}

void print_dirs_only(const std::unique_ptr<filenode::FileNode> &node, uintmax_t total_size)
{
    if (node->is_directory()) {
        print_directory(node, total_size);
    }
}

nlohmann::json build_json_from_tree(const std::unique_ptr<filenode::FileNode> &node, uintmax_t total_size, int depth = 0)
{
    depth++;
    nlohmann::json j;

    j["filename"] = node->filename;

    if (node->is_file()) {
        j["filesize"] = node->get_filesize();
        j["usage"] = utils::compute_relative_usage(node->get_filesize(), total_size);
    } else if (node->is_directory()) {
        j["filesize"] = node->get_filesize();
        j["usage"] = utils::compute_relative_usage(node->get_filesize(), total_size);
    } else {
        j["filesize"] = nullptr;
        j["usage"] = nullptr;
    }

    j["children"] = nlohmann::json::array();

    for (const auto &child: node->children) {
        if (skip_level(depth)) {
            continue;
        }

        if (params::EXCLUDES.contains(child->filename)) {
            continue;
        }
        j["children"].push_back(build_json_from_tree(child, total_size, depth));
    }

    return j;
}

} // namespace

namespace reporting {

void print_pretty_output(const std::unique_ptr<filenode::FileNode> &node, uintmax_t total_size, int depth, const std::string &prefix, bool is_last)
{
    fmt::print("{}", prefix);
    fmt::print("{}", (is_last ? "└── " : "├── "));
    print_dirs_files_or_other(node, total_size);
    depth++;

    std::string next_prefix = prefix + (is_last ? "    " : "│   ");

    for (size_t i = 0; i < node->children.size(); ++i) {
        if (skip_level(depth)) {
            continue;
        }

        if (params::EXCLUDES.contains(node->children[i]->filename)) {
            continue;
        }
        print_pretty_output(node->children[i], total_size, depth, next_prefix, i == node->children.size() - 1);
    }
}

void print_pretty_output_dirs_only(const std::unique_ptr<filenode::FileNode> &node, uintmax_t total_size, int depth, const std::string &prefix, bool is_last)
{
    fmt::print("{}", prefix);
    fmt::print("{}", (is_last ? "└── " : "├── "));
    print_dirs_only(node, total_size);
    depth++;

    std::string next_prefix = prefix + (is_last ? "    " : "│   ");

    for (size_t i = 0; i < node->children.size(); ++i) {
        if (skip_level(depth)) {
            continue;
        }

        if (params::EXCLUDES.contains(node->children[i]->filename)) {
            continue;
        }
        print_pretty_output_dirs_only(node->children[i], total_size, depth, next_prefix, i == node->children.size() - 1);
    }
}

void print_json(const std::unique_ptr<filenode::FileNode> &node, uintmax_t total_size)
{
    nlohmann::json json = build_json_from_tree(node, total_size);
    fmt::print("{}\n", json.dump(params::INDENT_LEVEL));
}

} // namespace reporting
