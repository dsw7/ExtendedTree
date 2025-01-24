#include "reporting.hpp"

#include "utils.hpp"

#include <fmt/color.h>
#include <fmt/core.h>
#include <map>
#include <string>

namespace {

std::map<int, std::string> ws;

constexpr fmt::terminal_color blue = fmt::terminal_color::bright_blue;
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
    fmt::print(fg(green), "( {} B )", size);
}

inline float get_relative_size(uintmax_t size, uintmax_t total_size)
{
    return ((float)size / total_size) * 100;
}

void print_relative_usage(uintmax_t size, uintmax_t total_size)
{
    fmt::print(fg(green), "[ {:.{}f}% ]", get_relative_size(size, total_size), 2);
}

} // namespace

namespace reporting {

void print_dirs_only(const std::unique_ptr<FileNode> &node, int depth)
{
    cache_whitespace(depth);

    if (node->filetype == DIRECTORY) {
        fmt::print(fg(blue), "{}{}/ ", ws[depth], node->filename);
        print_absolute_usage(node->filesize.value());
        fmt::print("\n");
    }
}

void print_dirs_only(const std::unique_ptr<FileNode> &node, int depth, uintmax_t total_size)
{
    cache_whitespace(depth);

    if (node->filetype == DIRECTORY) {
        fmt::print(fg(blue), "{}{}/ ", ws[depth], node->filename);
        print_relative_usage(node->filesize.value(), total_size);
        fmt::print("\n");
    }
}

} // namespace reporting
