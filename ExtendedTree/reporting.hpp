#pragma once

#include "filenode.hpp"

#include <memory>

namespace reporting {

void print_relative(const std::unique_ptr<filenode::FileNode> &node, uintmax_t total_size, int depth = 0);
void print_relative_dirs(const std::unique_ptr<filenode::FileNode> &node, uintmax_t total_size, int depth = 0);
void print_json(const std::unique_ptr<filenode::FileNode> &node);
void print_json(const std::unique_ptr<filenode::FileNode> &node, uintmax_t total_size);

} // namespace reporting
