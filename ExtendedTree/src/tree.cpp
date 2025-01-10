#include "tree.hpp"

#include <iostream>

namespace fs = std::filesystem;

void run_tree(const fs::path &dir)
{
    for (auto it = fs::recursive_directory_iterator(dir); it != fs::recursive_directory_iterator(); ++it) {
        std::cout << it.depth() << ' ' << *it << '\n';
    }
}
