#include "report_pretty.hpp"

#include "utils.hpp"

#include <fmt/color.h>
#include <fmt/core.h>
#include <map>
#include <string>

namespace {

std::map<int, std::string> WHITESPACE;

constexpr fmt::terminal_color blue = fmt::terminal_color::bright_blue;
constexpr fmt::terminal_color cyan = fmt::terminal_color::bright_cyan;
constexpr fmt::terminal_color green = fmt::terminal_color::bright_green;

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
    fmt::print(fg(green), "( {} B )\n", size);
}

void print_relative_usage(uintmax_t size, uintmax_t total_size)
{
    float relative_size = compute_relative_usage(size, total_size);
    fmt::print(fg(green), "[ {:.{}f}% ]\n", relative_size, 2);
}

void print(const std::unique_ptr<FileNode> &node, int depth)
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

void print(const std::unique_ptr<FileNode> &node, int depth, uintmax_t total_size)
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

void print_dirs_only(const std::unique_ptr<FileNode> &node, int depth)
{
    cache_whitespace(depth);

    if (node->is_directory()) {
        print_directory(node->filename, depth);
        print_absolute_usage(node->get_filesize());
    }
}

void print_dirs_only(const std::unique_ptr<FileNode> &node, int depth, uintmax_t total_size)
{
    cache_whitespace(depth);

    if (node->is_directory()) {
        print_directory(node->filename, depth);
        print_relative_usage(node->get_filesize(), total_size);
    }
}

} // namespace

namespace reporting {

void print_relative(const std::unique_ptr<FileNode> &node, uintmax_t total_size, int depth)
{
    print(node, depth, total_size);
    depth++;

    for (const auto &child: node->children) {
        print_relative(child, total_size, depth);
    }
}

void print_absolute(const std::unique_ptr<FileNode> &node, int depth)
{
    print(node, depth);
    depth++;

    for (const auto &child: node->children) {
        print_absolute(child, depth);
    }
}

void print_relative_dirs(const std::unique_ptr<FileNode> &node, uintmax_t total_size, int depth)
{
    print_dirs_only(node, depth, total_size);
    depth++;

    for (const auto &child: node->children) {
        print_relative_dirs(child, total_size, depth);
    }
}

void print_absolute_dirs(const std::unique_ptr<FileNode> &node, int depth)
{
    print_dirs_only(node, depth);
    depth++;

    for (const auto &child: node->children) {
        print_absolute_dirs(child, depth);
    }
}

} // namespace reporting
