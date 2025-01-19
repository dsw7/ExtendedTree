#include "filenode.hpp"

FileNode::FileNode(const std::string &filename, const FileType filetype, const std::optional<uintmax_t> &filesize)
{
    this->filename = filename;
    this->filetype = filetype;
    this->filesize = filesize;
}
