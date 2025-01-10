#include "tree.hpp"

#include <filesystem>
#include <iostream>

namespace fs = std::filesystem;

void explore()
{
    fs::path target = fs::current_path();
    run_tree(target);
}

int main()
{
    try {
        explore();
    } catch (const fs::filesystem_error &e) {
        std::cerr << "Error: " << e.what() << '\n';
        return EXIT_FAILURE;
    }

    return 0;
}
