#include "utils.hpp"

#include <cmath>
#include <stdexcept>
#include <sys/stat.h>

namespace utils {

void strip_extra_path_delimiter(std::string &path)
{
    if (path.size() < 2) {
        return;
    }

    if (path.back() == '/') {
        path.pop_back();
    }
}

float compute_relative_usage(uintmax_t size, uintmax_t total_size)
{
    if (total_size == 0) {
        return 0;
    }

    return ((float)size / total_size) * 100;
}

uintmax_t get_disk_usage(const std::string &path)
{
    struct stat buf_filestats;

    if (stat(path.c_str(), &buf_filestats) != 0) {
        throw std::runtime_error("Cannot get disk usage for file '" + path + "'");
    }

    return static_cast<uintmax_t>(buf_filestats.st_blocks) * 512; // Convert to bytes
}

std::string bytes_to_human(uintmax_t bytes)
{
    static uintmax_t kb = 1000;
    static uintmax_t mb = std::pow(1000, 2);
    static uintmax_t gb = std::pow(1000, 3);
    static uintmax_t tb = std::pow(1000, 4);

    if (bytes < kb) {
        return std::to_string(bytes);
    }

    std::string result;

    if (bytes >= kb && bytes < mb) {
        result = std::to_string((bytes + kb - 1) / kb) + 'K';
    } else if (bytes >= mb && bytes < gb) {
        result = std::to_string((bytes + mb - 1) / mb) + 'M';
    } else if (bytes >= gb && bytes < tb) {
        result = std::to_string((bytes + gb - 1) / gb) + 'G';
    } else {
        result = std::to_string((bytes + tb - 1) / tb) + 'T';
    }

    return result;
}

} // namespace utils
