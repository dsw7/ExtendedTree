#include "tree.hpp"

#include "params.hpp"

#include <filesystem>
#include <iostream>
#include <stdexcept>

int main(int argc, char **argv)
{
    const Params params = parse_cli(argc, argv);

    try {
        run_tree(params);
    } catch (const std::filesystem::filesystem_error &e) {
        std::cerr << "Error: " << e.what() << '\n';
        return EXIT_FAILURE;
    } catch (const std::runtime_error &e) {
        std::cerr << "Error: " << e.what() << '\n';
        return EXIT_FAILURE;
    }

    return 0;
}
