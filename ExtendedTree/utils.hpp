#pragma once

#include <string>

const int TAB_WIDTH = 4;

namespace utils {
void strip_extra_path_delimiter(std::string &path);
float compute_relative_usage(uintmax_t size, uintmax_t total_size);
uintmax_t get_disk_usage(const std::string &path);
std::string bytes_to_human(uint64_t bytes);
} // namespace utils
