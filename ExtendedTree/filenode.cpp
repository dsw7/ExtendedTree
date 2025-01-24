#include "filenode.hpp"

#include "utils.hpp"

#include <cmath>
#include <fmt/color.h>
#include <fmt/core.h>
#include <map>

namespace {

constexpr fmt::terminal_color blue = fmt::terminal_color::bright_blue;
constexpr fmt::terminal_color cyan = fmt::terminal_color::bright_cyan;
constexpr fmt::terminal_color green = fmt::terminal_color::bright_green;

std::map<int, std::string> ws;

void cache_whitespace(int depth)
{
    if (!ws.contains(depth)) {
        ws.emplace(depth, std::string(depth * TAB_WIDTH, ' '));
    }
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

FileNode::FileNode(const std::string &filename, const FileType filetype, const std::optional<uintmax_t> &filesize)
{
    this->filename = filename;
    this->filetype = filetype;
    this->filesize = filesize;
}

void FileNode::print(int depth)
{
    cache_whitespace(depth);

    switch (this->filetype) {
        case REGULAR_FILE:
            fmt::print("{}{} ", ws[depth], this->filename);
            print_absolute_usage(this->filesize.value());
            fmt::print("\n");
            break;
        case DIRECTORY:
            fmt::print(fg(blue), "{}{}/ ", ws[depth], this->filename);
            print_absolute_usage(this->filesize.value());
            fmt::print("\n");
            break;
        case OTHER:
            fmt::print(fg(cyan), "{}{} ?\n", ws[depth], this->filename);
            break;
        default:
            fmt::print(fg(cyan), "{}{} ?\n", ws[depth], this->filename);
    }
}

void FileNode::print(int depth, uintmax_t total_size)
{
    cache_whitespace(depth);

    switch (this->filetype) {
        case REGULAR_FILE:
            fmt::print("{}{} ", ws[depth], this->filename);
            print_relative_usage(this->filesize.value(), total_size);
            fmt::print("\n");
            break;
        case DIRECTORY:
            fmt::print(fg(blue), "{}{}/ ", ws[depth], this->filename);
            print_relative_usage(this->filesize.value(), total_size);
            fmt::print("\n");
            break;
        case OTHER:
            fmt::print(fg(cyan), "{}{} ?\n", ws[depth], this->filename);
            break;
        default:
            fmt::print(fg(cyan), "{}{} ?\n", ws[depth], this->filename);
    }
}

void FileNode::print_dirs_only(int depth)
{
    cache_whitespace(depth);

    if (this->filetype == DIRECTORY) {
        fmt::print(fg(blue), "{}{}/ ", ws[depth], this->filename);
        print_absolute_usage(this->filesize.value());
        fmt::print("\n");
    }
}
