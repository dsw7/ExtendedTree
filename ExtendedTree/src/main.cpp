#include "tree.hpp"

#include <filesystem>
#include <fmt/core.h>
#include <iostream>
#include <optional>
#include <unistd.h>

namespace fs = std::filesystem;

struct Params {
    std::optional<std::string> level = std::nullopt;
    std::optional<fs::path> target = std::nullopt;
};

void parse_cli(int argc, char **argv, Params &params)
{
    int c = 0;

    while ((c = getopt(argc, argv, "L:")) != -1) {
        switch (c) {
            case 'L':
                params.level = optarg;
                break;
            default:
                std::cerr << "Error parsing command line\n";
                exit(EXIT_FAILURE);
        }
    }

    for (int i = optind; i < argc; i++) {
        params.target = fs::path(argv[i]);
        break;
    }
}

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
    Params params;
    parse_cli(argc, argv, params);

    try {
        explore(params);
    } catch (const fs::filesystem_error &e) {
        std::cerr << "Error: " << e.what() << '\n';
        return EXIT_FAILURE;
    }

    return 0;
}
