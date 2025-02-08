#include "utils.hpp"

float compute_relative_usage(uintmax_t size, uintmax_t total_size)
{
    return ((float)size / total_size) * 100;
}

void strip_extra_path_delimiter(std::string &path)
{
    if (path.size() < 2) {
        return;
    }

    if (path.back() == '/') {
        path.pop_back();
    }
}
