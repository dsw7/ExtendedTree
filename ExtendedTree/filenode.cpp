#include "filenode.hpp"

FileNode::FileNode(const std::string &filename, const FileType filetype, const std::optional<uintmax_t> &filesize)
{
    this->filename = filename;
    this->filetype = filetype;
    this->filesize = filesize;
}

void FileNode::set_is_file()
{
    this->type = REGULAR_FILE;
}

void FileNode::set_is_directory()
{
    this->type = DIRECTORY;
}

void FileNode::set_is_other()
{
    this->type = OTHER;
}

bool FileNode::is_file()
{
    return this->type == REGULAR_FILE;
}

bool FileNode::is_directory()
{
    return this->type == DIRECTORY;
}

bool FileNode::is_other()
{
    return this->type == OTHER;
}
