#include "reporting.hpp"

#include "utils.hpp"

#include <fmt/color.h>
#include <fmt/core.h>
#include <map>
#include <string>

namespace {

std::map<int, std::string> ws;

constexpr fmt::terminal_color blue = fmt::terminal_color::bright_blue;
constexpr fmt::terminal_color cyan = fmt::terminal_color::bright_cyan;
constexpr fmt::terminal_color green = fmt::terminal_color::bright_green;

void cache_whitespace(int depth)
{
    if (ws.contains(depth)) {
        return;
    }

    ws.emplace(depth, std::string(depth * TAB_WIDTH, ' '));
}

void print_absolute_usage(uintmax_t size)
{
    fmt::print(fg(green), "( {} B )\n", size);
}

void print_relative_usage(uintmax_t size, uintmax_t total_size)
{
    float relative_size = ((float)size / total_size) * 100;
    fmt::print(fg(green), "[ {:.{}f}% ]\n", relative_size, 2);
}

} // namespace

namespace reporting {

void print(const std::unique_ptr<FileNode> &node, int depth)
{
    cache_whitespace(depth);

    switch (node->filetype) {
        case REGULAR_FILE:
            fmt::print("{}{} ", ws[depth], node->filename);
            print_absolute_usage(node->filesize.value());
            break;
        case DIRECTORY:
            fmt::print(fg(blue), "{}{}/ ", ws[depth], node->filename);
            print_absolute_usage(node->filesize.value());
            break;
        case OTHER:
            fmt::print(fg(cyan), "{}{} ?\n", ws[depth], node->filename);
            break;
        default:
            fmt::print(fg(cyan), "{}{} ?\n", ws[depth], node->filename);
    }
}

void print(const std::unique_ptr<FileNode> &node, int depth, uintmax_t total_size)
{
    cache_whitespace(depth);

    switch (node->filetype) {
        case REGULAR_FILE:
            fmt::print("{}{} ", ws[depth], node->filename);
            print_relative_usage(node->filesize.value(), total_size);
            break;
        case DIRECTORY:
            fmt::print(fg(blue), "{}{}/ ", ws[depth], node->filename);
            print_relative_usage(node->filesize.value(), total_size);
            break;
        case OTHER:
            fmt::print(fg(cyan), "{}{} ?\n", ws[depth], node->filename);
            break;
        default:
            fmt::print(fg(cyan), "{}{} ?\n", ws[depth], node->filename);
    }
}

void print_dirs_only(const std::unique_ptr<FileNode> &node, int depth)
{
    cache_whitespace(depth);

    if (node->filetype == DIRECTORY) {
        fmt::print(fg(blue), "{}{}/ ", ws[depth], node->filename);
        print_absolute_usage(node->filesize.value());
    }
}

void print_dirs_only(const std::unique_ptr<FileNode> &node, int depth, uintmax_t total_size)
{
    cache_whitespace(depth);

    if (node->filetype == DIRECTORY) {
        fmt::print(fg(blue), "{}{}/ ", ws[depth], node->filename);
        print_relative_usage(node->filesize.value(), total_size);
    }
}

} // namespace reporting
