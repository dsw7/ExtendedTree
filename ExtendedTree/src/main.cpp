#include "tree.hpp"

#include "utils.hpp"

#include <filesystem>

int main()
{
    std::filesystem::path target = std::filesystem::current_path();
    print("Exploring directory: " + target.string());

    explore(target);
    return 0;
}
