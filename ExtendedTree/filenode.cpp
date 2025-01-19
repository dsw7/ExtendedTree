#include "filenode.hpp"

#include "utils.hpp"

#include <fmt/color.h>
#include <fmt/core.h>
#include <map>

namespace {

constexpr fmt::terminal_color blue = fmt::terminal_color::bright_blue;
constexpr fmt::terminal_color green = fmt::terminal_color::bright_green;

} // namespace

FileNode::FileNode(const std::string &filename, const FileType filetype, const std::optional<uintmax_t> &filesize)
{
    this->filename = filename;
    this->filetype = filetype;
    this->filesize = filesize;
}

void FileNode::print(int depth)
{
    static std::map<int, std::string> ws;

    if (!ws.contains(depth)) {
        ws.emplace(depth, std::string(depth * TAB_WIDTH, ' '));
    }

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
