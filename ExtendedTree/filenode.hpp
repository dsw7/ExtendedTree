#pragma once

#include <memory>
#include <optional>
#include <string>
#include <vector>

enum FileType {
    REGULAR_FILE,
    DIRECTORY,
    OTHER,
};

struct FileNode {
    FileType filetype = REGULAR_FILE;

    std::optional<uintmax_t> filesize = std::nullopt;
    std::string filename;
    std::vector<std::unique_ptr<FileNode>> children;

    FileNode(const std::string &filename, const FileType filetype, const std::optional<uintmax_t> &filesize);
    void print(int depth);
};
