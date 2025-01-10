#include "tree.hpp"

#include "utils.hpp"

#include <filesystem>
#include <fmt/core.h>

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
        print_error(e.what());
        return EXIT_FAILURE;
    }

    return 0;
}
