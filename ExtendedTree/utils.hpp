#pragma once

#include <string>

const int TAB_WIDTH = 4;

void strip_extra_path_delimiter(std::string &path);
float compute_relative_usage(uintmax_t size, uintmax_t total_size);
std::string bytes_to_human(uintmax_t bytes);
