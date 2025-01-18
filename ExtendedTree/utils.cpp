#include "utils.hpp"

#include <fmt/core.h>
#include <string>

namespace {

const std::string bl_elbow = "└";
const std::string br_elbow = "┘";
const std::string hline = "─";
const std::string htee = "┴";

} // namespace

void print_ruler(int depth)
{
    if (depth < 1) {
        return;
    }

    std::string scale;
    std::string ruler;

    int max = depth * TAB_WIDTH;
    int inc = 0;

    for (int i = 0; i <= max; i++) {
        if (i == 0) {
            scale += std::to_string(0);
            ruler += bl_elbow;
            continue;
        }

        if (i == max) {
            scale += std::to_string(++inc);
            ruler += br_elbow;
            continue;
        }

        if (i % 4 == 0) {
            scale += std::to_string(++inc);
            ruler += htee;
        } else {
            scale += ' ';
            ruler += hline;
        }
    }

    fmt::print("{}\n{}\n", scale, ruler);
}
