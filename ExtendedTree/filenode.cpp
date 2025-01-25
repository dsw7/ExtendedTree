#include "filenode.hpp"

FileNode::FileNode(const std::string &filename)
{
    this->filename = filename;
}

void FileNode::set_filesize(const std::optional<uintmax_t> &filesize)
{
    this->filesize = filesize;
}

std::optional<uintmax_t> FileNode::get_filesize()
{
    return this->filesize;
}

void FileNode::set_is_file()
{
    this->filetype = REGULAR_FILE;
}

void FileNode::set_is_directory()
{
    this->filetype = DIRECTORY;
}

void FileNode::set_is_other()
{
    this->filetype = OTHER;
}

bool FileNode::is_file()
{
    return this->filetype == REGULAR_FILE;
}

bool FileNode::is_directory()
{
    return this->filetype == DIRECTORY;
}

bool FileNode::is_other()
{
    return this->filetype == OTHER;
}
