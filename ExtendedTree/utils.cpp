#include "utils.hpp"

#include <cmath>
#include <iomanip>
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

std::string bytes_to_human(uint64_t bytes)
{
    constexpr uint64_t KIB = 1024;
    constexpr uint64_t MIB = KIB * 1024;
    constexpr uint64_t GIB = MIB * 1024;
    constexpr uint64_t TIB = GIB * 1024;

    double value;
    std::string unit;

    if (bytes >= TIB) {
        value = static_cast<double>(bytes) / TIB;
        unit = "TiB";
    } else if (bytes >= GIB) {
        value = static_cast<double>(bytes) / GIB;
        unit = "GiB";
    } else if (bytes >= MIB) {
        value = static_cast<double>(bytes) / MIB;
        unit = "MiB";
    } else if (bytes >= KIB) {
        value = static_cast<double>(bytes) / KIB;
        unit = "KiB";
    } else {
        value = static_cast<double>(bytes);
        unit = "B";
    }

    std::ostringstream oss;
    oss << std::fixed << std::setprecision(2) << value << ' ' << unit;

    return oss.str();
}

} // namespace utils
