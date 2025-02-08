#pragma once

#include "filenode.hpp"

#include <memory>

namespace reporting {

void print_json(const std::unique_ptr<FileNode> &node);
void print_json(const std::unique_ptr<FileNode> &node, uintmax_t total_size);

} // namespace reporting
