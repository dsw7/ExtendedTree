#include "reporting.hpp"

#include "params.hpp"
#include "utils.hpp"

#include <fmt/color.h>
#include <fmt/core.h>
#include <json.hpp>
#include <map>
#include <string>

namespace {

std::map<int, std::string> WHITESPACE;

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

void cache_whitespace(int depth)
{
    if (WHITESPACE.contains(depth)) {
        return;
    }

    WHITESPACE.emplace(depth, std::string(depth * TAB_WIDTH, ' '));
}

void print_file(const std::string &filename, int depth)
{
    fmt::print("{}{} ", WHITESPACE[depth], filename);
}

void print_directory(const std::string &filename, int depth)
{
    fmt::print(fg(blue), "{}{}/ ", WHITESPACE[depth], filename);
}

void print_other(const std::string &filename, int depth)
{
    fmt::print(fg(cyan), "{}{} ?\n", WHITESPACE[depth], filename);
}

void print_absolute_usage(uintmax_t size)
{
    if (params::PRINT_BYTES) {
        fmt::print(fg(green), "{}\n", size);
    } else {
        fmt::print(fg(green), "{}\n", utils::bytes_to_human(size));
    }
}

void print_relative_usage(uintmax_t size, uintmax_t total_size)
{
    float relative_size = utils::compute_relative_usage(size, total_size);
    fmt::print(fg(green), "[ {:.{}f}% ]\n", relative_size, 2);
}

void print(const std::unique_ptr<filenode::FileNode> &node, int depth)
{
    cache_whitespace(depth);

    if (node->is_file()) {
        print_file(node->filename, depth);
        print_absolute_usage(node->get_filesize());
    } else if (node->is_directory()) {
        print_directory(node->filename, depth);
        print_absolute_usage(node->get_filesize());
    } else {
        print_other(node->filename, depth);
    }
}

void print(const std::unique_ptr<filenode::FileNode> &node, int depth, uintmax_t total_size)
{
    cache_whitespace(depth);

    if (node->is_file()) {
        print_file(node->filename, depth);
        print_relative_usage(node->get_filesize(), total_size);
    } else if (node->is_directory()) {
        print_directory(node->filename, depth);
        print_relative_usage(node->get_filesize(), total_size);
    } else {
        print_other(node->filename, depth);
    }
}

void print_dirs_only(const std::unique_ptr<filenode::FileNode> &node, int depth)
{
    cache_whitespace(depth);

    if (node->is_directory()) {
        print_directory(node->filename, depth);
        print_absolute_usage(node->get_filesize());
    }
}

void print_dirs_only(const std::unique_ptr<filenode::FileNode> &node, int depth, uintmax_t total_size)
{
    cache_whitespace(depth);

    if (node->is_directory()) {
        print_directory(node->filename, depth);
        print_relative_usage(node->get_filesize(), total_size);
    }
}

nlohmann::json build_json_from_tree(const std::unique_ptr<filenode::FileNode> &node)
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
        j["children"].push_back(build_json_from_tree(child));
    }

    return j;
}

nlohmann::json build_json_from_tree(const std::unique_ptr<filenode::FileNode> &node, uintmax_t total_size)
{
    nlohmann::json j;

    j["filename"] = node->filename;

    if (node->is_file()) {
        j["filesize"] = utils::compute_relative_usage(node->get_filesize(), total_size);
    } else if (node->is_directory()) {
        j["filesize"] = utils::compute_relative_usage(node->get_filesize(), total_size);
    } else {
        j["filesize"] = nullptr;
    }

    j["children"] = nlohmann::json::array();

    for (const auto &child: node->children) {
        j["children"].push_back(build_json_from_tree(child, total_size));
    }

    return j;
}

} // namespace

namespace reporting {

void print_relative(const std::unique_ptr<filenode::FileNode> &node, uintmax_t total_size, int depth)
{
    print(node, depth, total_size);
    depth++;

    for (const auto &child: node->children) {
        if (skip_level(depth)) {
            continue;
        }
        print_relative(child, total_size, depth);
    }
}

void print_absolute(const std::unique_ptr<filenode::FileNode> &node, int depth)
{
    print(node, depth);
    depth++;

    for (const auto &child: node->children) {
        if (skip_level(depth)) {
            continue;
        }
        print_absolute(child, depth);
    }
}

void print_relative_dirs(const std::unique_ptr<filenode::FileNode> &node, uintmax_t total_size, int depth)
{
    print_dirs_only(node, depth, total_size);
    depth++;

    for (const auto &child: node->children) {
        if (skip_level(depth)) {
            continue;
        }
        print_relative_dirs(child, total_size, depth);
    }
}

void print_absolute_dirs(const std::unique_ptr<filenode::FileNode> &node, int depth)
{
    print_dirs_only(node, depth);
    depth++;

    for (const auto &child: node->children) {
        if (skip_level(depth)) {
            continue;
        }
        print_absolute_dirs(child, depth);
    }
}

void print_json(const std::unique_ptr<filenode::FileNode> &node)
{
    nlohmann::json json = build_json_from_tree(node);
    fmt::print("{}\n", json.dump(params::INDENT_LEVEL));
}

void print_json(const std::unique_ptr<filenode::FileNode> &node, uintmax_t total_size)
{
    nlohmann::json json = build_json_from_tree(node, total_size);
    fmt::print("{}\n", json.dump(params::INDENT_LEVEL));
}

} // namespace reporting
