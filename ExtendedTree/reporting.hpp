#pragma once

#include "filenode.hpp"

#include <memory>

namespace reporting {

void print_dirs_only(const std::unique_ptr<FileNode> &node, int depth);
void print_dirs_only(const std::unique_ptr<FileNode> &node, int depth, uintmax_t total_size);

} // namespace reporting
