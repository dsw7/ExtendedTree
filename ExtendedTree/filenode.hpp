#pragma once

#include <filesystem>
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
    std::optional<uintmax_t> disk_usage_ = std::nullopt;
    std::optional<uintmax_t> num_children_ = std::nullopt;

public:
    std::string filename;
    std::vector<std::unique_ptr<FileNode>> children;

    FileNode(const std::filesystem::path &path);

    void set_disk_usage(uintmax_t disk_usage);
    uintmax_t get_disk_usage();
    void set_num_children(uintmax_t num_children);
    uintmax_t get_num_children();
    void set_is_file();
    void set_is_directory();
    void set_is_other();
    bool is_file();
    bool is_directory();
    bool is_other();
};

} // namespace filenode
