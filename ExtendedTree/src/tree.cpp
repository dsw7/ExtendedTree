#include "tree.hpp"

#include "utils.hpp"
#include <fmt/core.h>

namespace fs = std::filesystem;

void explore(const fs::path &dir)
{
    int depth = 0;

    for (auto it = fs::recursive_directory_iterator(dir); it != fs::recursive_directory_iterator(); ++it) {
        depth = it.depth();

        fmt::print("{} {}\n", depth, *it);
    }
}
