#pragma once

#include <string>

const int TAB_WIDTH = 4;

float compute_relative_usage(uintmax_t size, uintmax_t total_size);
void strip_extra_path_delimiter(std::string &path);
