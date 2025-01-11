#include "tree.hpp"

#include "params.hpp"

#include <filesystem>
#include <iostream>

int main(int argc, char **argv)
{
    Params params = parse_cli(argc, argv);

    try {
        run_tree(params);
    } catch (const std::filesystem::filesystem_error &e) {
        std::cerr << "Error: " << e.what() << '\n';
        return EXIT_FAILURE;
    }

    return 0;
}
