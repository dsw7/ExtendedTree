#include "tree.hpp"

#include "params.hpp"

#include <filesystem>
#include <fmt/core.h>
#include <iostream>

namespace fs = std::filesystem;

void explore(const Params &params)
{
    fs::path target;

    if (params.target.has_value()) {
        target = params.target.value();
    } else {
        target = fs::current_path();
    }

    if (!fs::exists(target)) {
        std::cerr << fmt::format("Directory '{}' does not exist\n", target.string());
        exit(EXIT_FAILURE);
    }

    if (!fs::is_directory(target)) {
        std::cerr << fmt::format("'{}' is not a directory\n", target.string());
        exit(EXIT_FAILURE);
    }

    run_tree(target);
}

int main(int argc, char **argv)
{
    Params params = parse_cli(argc, argv);

    try {
        explore(params);
    } catch (const fs::filesystem_error &e) {
        std::cerr << "Error: " << e.what() << '\n';
        return EXIT_FAILURE;
    }

    return 0;
}
