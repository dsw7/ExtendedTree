#include "utils.hpp"

#include <fmt/core.h>

namespace {

const std::string hline = "─";

void get_top_ruler(int length, std::string &scale, std::string &ruler)
{
    static std::string elbow_l = "└";
    static std::string elbow_r = "┘";
    static std::string tee = "┴";

    int inc = 0;

    for (int i = 0; i <= length; i++) {
        if (i == 0) {
            scale += std::to_string(0);
            ruler += elbow_l;
            continue;
        }

        if (i == length) {
            scale += std::to_string(++inc);
            ruler += elbow_r;
            continue;
        }

        if (i % 4 == 0) {
            scale += std::to_string(++inc);
            ruler += tee;
        } else {
            scale += ' ';
            ruler += hline;
        }
    }
}

void get_bottom_ruler(int length, std::string &scale, std::string &ruler)
{
    static std::string elbow_l = "┌";
    static std::string elbow_r = "┐";
    static std::string tee = "┬";

    int inc = 0;

    for (int i = 0; i <= length; i++) {
        if (i == 0) {
            scale += std::to_string(0);
            ruler += elbow_l;
            continue;
        }

        if (i == length) {
            scale += std::to_string(++inc);
            ruler += elbow_r;
            continue;
        }

        if (i % 4 == 0) {
            scale += std::to_string(++inc);
            ruler += tee;
        } else {
            scale += ' ';
            ruler += hline;
        }
    }
}

} // namespace

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

void print_ruler(int depth, bool top)
{
    if (depth < 1) {
        return;
    }

    std::string scale;
    std::string ruler;

    int length = depth * TAB_WIDTH;

    if (top) {
        get_top_ruler(length, scale, ruler);
        fmt::print("{}\n{}\n", scale, ruler);
    } else {
        get_bottom_ruler(length, scale, ruler);
        fmt::print("{}\n{}\n", ruler, scale);
    }
}
