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

void append_file(std::string &line, const std::string &filename)
{
    line += fmt::format("{} ", filename);
}

void append_directory(std::string &line, const std::string &filename)
{
    line += fmt::format(fg(blue), "{}/ ", filename);
}

void append_other(std::string &line, const std::string &filename)
{
    line += fmt::format(fg(cyan), "{} ?", filename);
}

void append_usage(std::string &line, uintmax_t size, uintmax_t total_size)
{
    float relative_size = utils::compute_relative_usage(size, total_size);

    if (params::PRINT_BYTES) {
        line += fmt::format(fg(green), "[ {} B, {:.{}f}% ]", size, relative_size, 2);
    } else {
        line += fmt::format(fg(green), "[ {}, {:.{}f}% ]", utils::bytes_to_human(size), relative_size, 2);
    }
}

void append_usage(std::string &line, uintmax_t size, uintmax_t total_size, uintmax_t num_children)
{
    float relative_size = utils::compute_relative_usage(size, total_size);

    if (params::PRINT_BYTES) {
        line += fmt::format(fg(green), "[ {} B, {:.{}f}%, {} ]", size, relative_size, 2, num_children);
    } else {
        line += fmt::format(fg(green), "[ {}, {:.{}f}%, {} ]", utils::bytes_to_human(size), relative_size, 2, num_children);
    }
}

nlohmann::json build_json_from_tree(const std::unique_ptr<filenode::FileNode> &node, uintmax_t total_size, int depth = 0)
{
    depth++;
    nlohmann::json j;

    if (node->is_file()) {
        j["filename"] = node->filename;
        j["disk_usage"] = node->get_disk_usage();
        j["relative_usage"] = utils::compute_relative_usage(node->get_disk_usage(), total_size);
    } else if (node->is_directory()) {
        j["dirname"] = node->filename;
        j["num_children"] = node->get_num_children();
        j["disk_usage"] = node->get_disk_usage();
        j["relative_usage"] = utils::compute_relative_usage(node->get_disk_usage(), total_size);
    } else {
        j["filename"] = node->filename;
        j["disk_usage"] = nullptr;
        j["relative_usage"] = nullptr;
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
    if (params::EXCLUDES.contains(node->filename)) {
        return;
    }

    std::string next_prefix;
    std::string line = prefix;

    if (is_last) {
        line += "└── ";
        next_prefix = prefix + "    ";
    } else {
        line += "├── ";
        next_prefix = prefix + "│   ";
    }

    if (node->is_file()) {
        append_file(line, node->filename);
        append_usage(line, node->get_disk_usage(), total_size);
    } else if (node->is_directory()) {
        append_directory(line, node->filename);
        append_usage(line, node->get_disk_usage(), total_size, node->get_num_children());
    } else {
        append_other(line, node->filename);
    }

    fmt::print("{}\n", line);

    depth++;
    if (skip_level(depth)) {
        return;
    }

    size_t num_children = node->children.size();

    for (size_t i = 0; i < num_children; ++i) {
        print_pretty_output(node->children[i], total_size, depth, next_prefix, i == num_children - 1);
    }
}

void print_pretty_output_dirs_only(const std::unique_ptr<filenode::FileNode> &node, uintmax_t total_size, int depth, const std::string &prefix, bool is_last)
{
    if (params::EXCLUDES.contains(node->filename)) {
        return;
    }

    std::string next_prefix;

    if (node->is_directory()) {
        std::string line = prefix;

        if (is_last) {
            line += "└── ";
            next_prefix = prefix + "    ";
        } else {
            line += "├── ";
            next_prefix = prefix + "│   ";
        }

        append_directory(line, node->filename);
        append_usage(line, node->get_disk_usage(), total_size, node->get_num_children());
        fmt::print("{}\n", line);
    } else {
        next_prefix = "";
    }

    depth++;
    if (skip_level(depth)) {
        return;
    }

    size_t num_children = node->children.size();

    for (size_t i = 0; i < num_children; ++i) {
        print_pretty_output_dirs_only(node->children[i], total_size, depth, next_prefix, i == num_children - 1);
    }
}

void print_json(const std::unique_ptr<filenode::FileNode> &root)
{
    nlohmann::json json = build_json_from_tree(root, root->get_disk_usage());
    fmt::print("{}\n", json.dump(params::INDENT_LEVEL));
}

} // namespace reporting
