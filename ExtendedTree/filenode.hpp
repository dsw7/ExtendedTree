#pragma once

#include <memory>
#include <optional>
#include <string>
#include <vector>

namespace filenode {

enum FileType {
    REGULAR_FILE,
    DIRECTORY,
    OTHER,
};

class FileNode {
private:
    FileType filetype_ = REGULAR_FILE;
    std::optional<uintmax_t> filesize_ = std::nullopt;

public:
    std::string filename;
    std::vector<std::unique_ptr<FileNode>> children;

    FileNode(const std::string &filename);

    void set_filesize(uintmax_t filesize);
    uintmax_t get_filesize();
    void set_is_file();
    void set_is_directory();
    void set_is_other();
    bool is_file();
    bool is_directory();
    bool is_other();
};

} // namespace filenode
