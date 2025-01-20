#include "tree.hpp"

#include <filesystem>
#include <fmt/core.h>
#include <iostream>
#include <optional>
#include <stdexcept>
#include <string>
#include <unistd.h>

struct Options {
    bool absolute = false;
    std::optional<std::filesystem::path> target = std::nullopt;
    std::optional<std::string> level = std::nullopt;
};

void print_help_messages()
{
    fmt::print("Usage:\n  etree [OPTION]... DIRECTORY\n\n");
    fmt::print("Options:\n");
    fmt::print("  -a  {}\n", "Print usage in bytes (as opposed to percentage)");
    fmt::print("  -h  {}\n", "Print this help message and exit");
}

Options parse_cli_options(int argc, char **argv)
{
    Options options;
    int option = 0;

    while ((option = getopt(argc, argv, "haL:")) != -1) {

        switch (option) {
            case 'h':
                print_help_messages();
                exit(EXIT_SUCCESS);
            case 'a':
                options.absolute = true;
                break;
            case 'L':
                options.level = optarg;
                break;
            default:
                print_help_messages();
                exit(EXIT_FAILURE);
        }
    }

    for (int i = optind; i < argc; i++) {
        options.target = std::filesystem::path(argv[i]);
        break;
    }

    return options;
}

int main(int argc, char **argv)
{
    const Options options = parse_cli_options(argc, argv);

    try {
        run_tree({ options.absolute, options.target });
    } catch (const std::filesystem::filesystem_error &e) {
        std::cerr << "Error: " << e.what() << '\n';
        return EXIT_FAILURE;
    } catch (const std::runtime_error &e) {
        std::cerr << "Error: " << e.what() << '\n';
        return EXIT_FAILURE;
    }

    return 0;
}
