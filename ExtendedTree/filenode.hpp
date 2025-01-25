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

class FileNode {
private:
    FileType type = REGULAR_FILE;

public:
    FileType filetype = REGULAR_FILE;

    std::optional<uintmax_t> filesize = std::nullopt;
    std::string filename;
    std::vector<std::unique_ptr<FileNode>> children;

    FileNode(const std::string &filename, const FileType filetype, const std::optional<uintmax_t> &filesize);

    void set_is_file();
    void set_is_directory();
    void set_is_other();
    bool is_file();
    bool is_directory();
    bool is_other();
};
