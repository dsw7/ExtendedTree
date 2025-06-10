#pragma once

#include "filenode.hpp"

#include <memory>

namespace reporting {

void print_pretty_output(const std::unique_ptr<filenode::FileNode> &node, uintmax_t total_size, int depth = 0, const std::string &prefix = "", bool is_last = true);
void print_pretty_output_dirs_only(const std::unique_ptr<filenode::FileNode> &node, uintmax_t total_size, int depth = 0, const std::string &prefix = "", bool is_last = true);
void print_json(const std::unique_ptr<filenode::FileNode> &root);

} // namespace reporting
