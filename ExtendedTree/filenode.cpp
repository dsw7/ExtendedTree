#include "filenode.hpp"

#include "utils.hpp"

#include <fmt/color.h>
#include <fmt/core.h>
#include <map>

namespace {

constexpr fmt::terminal_color blue = fmt::terminal_color::bright_blue;
constexpr fmt::terminal_color green = fmt::terminal_color::bright_green;

std::map<int, std::string> ws;

void cache_whitespace(int depth)
{
    if (!ws.contains(depth)) {
        ws.emplace(depth, std::string(depth * TAB_WIDTH, ' '));
    }
}

float compute_relative_usage(int size, int total_size)
{
    return ((float)size / total_size) * 100;
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
            fmt::print("{}{} {}\n", ws[depth], this->filename, this->filesize.value());
            break;
        case DIRECTORY:
            fmt::print(fg(blue), "{}{}/ {}\n", ws[depth], this->filename, this->filesize.value());
            break;
        case OTHER:
            fmt::print(fg(green), "{}{} ?\n", ws[depth], this->filename);
            break;
        default:
            fmt::print(fg(green), "{}{} ?\n", ws[depth], this->filename);
    }
}

void FileNode::print(int depth, int total_size)
{
    cache_whitespace(depth);

    float relative_usage = 0;

    if (this->filetype == REGULAR_FILE || this->filetype == DIRECTORY) {
        relative_usage = compute_relative_usage(this->filesize.value(), total_size);
    }

    switch (this->filetype) {
        case REGULAR_FILE:
            fmt::print("{}{} {}\n", ws[depth], this->filename, relative_usage);
            break;
        case DIRECTORY:
            fmt::print(fg(blue), "{}{}/ {}\n", ws[depth], this->filename, relative_usage);
            break;
        case OTHER:
            fmt::print(fg(green), "{}{} ?\n", ws[depth], this->filename);
            break;
        default:
            fmt::print(fg(green), "{}{} ?\n", ws[depth], this->filename);
    }
}
