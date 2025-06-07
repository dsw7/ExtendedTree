#include "filenode.hpp"

namespace filenode {

FileNode::FileNode(const std::string &filename)
{
    this->filename = filename;
}

void FileNode::set_filesize(uintmax_t filesize)
{
    this->filesize_ = filesize;
}

uintmax_t FileNode::get_filesize()
{
    return this->filesize_.value_or(0);
}

void FileNode::set_is_file()
{
    this->filetype_ = REGULAR_FILE;
}

void FileNode::set_is_directory()
{
    this->filetype_ = DIRECTORY;
}

void FileNode::set_is_other()
{
    this->filetype_ = OTHER;
}

bool FileNode::is_file()
{
    return this->filetype_ == REGULAR_FILE;
}

bool FileNode::is_directory()
{
    return this->filetype_ == DIRECTORY;
}

bool FileNode::is_other()
{
    return this->filetype_ == OTHER;
}

} // namespace filenode
