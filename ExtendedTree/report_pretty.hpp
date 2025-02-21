#pragma once

#include "filenode.hpp"

#include <memory>

namespace reporting {

void print_relative(const std::unique_ptr<FileNode> &node, uintmax_t total_size, int depth = 0);
void print_absolute(const std::unique_ptr<FileNode> &node, int depth = 0);
void print_relative_dirs(const std::unique_ptr<FileNode> &node, uintmax_t total_size, int depth = 0);
void print_absolute_dirs(const std::unique_ptr<FileNode> &node, int depth = 0);

} // namespace reporting
