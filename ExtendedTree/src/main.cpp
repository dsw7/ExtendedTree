#include "tree.hpp"

#include <filesystem>

int main()
{
    std::filesystem::path target = std::filesystem::current_path();

    explore(target);
    return 0;
}
