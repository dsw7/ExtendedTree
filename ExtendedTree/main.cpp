#include "tree.hpp"

#include "utils.hpp"

#include <filesystem>
#include <fmt/core.h>
#include <iostream>
#include <optional>
#include <stdexcept>
#include <string>
#include <unistd.h>

namespace fs = std::filesystem;

struct Options {
    bool absolute = false;
    bool dirs_only = false;
    std::optional<fs::path> target = std::nullopt;
    std::optional<std::string> level = std::nullopt;
};

void print_help_messages()
{
    fmt::print("Usage:\n  etree [OPTION]... DIRECTORY\n\n");
    fmt::print("Options:\n");
    fmt::print("  -a  {}\n", "Print usage in bytes (as opposed to percentage)");
    fmt::print("  -d  {}\n", "Print directories only");
    fmt::print("  -h  {}\n", "Print this help message and exit");
}

Options parse_cli_options(int argc, char **argv)
{
    Options options;
    int option = 0;

    while ((option = getopt(argc, argv, "hadL:")) != -1) {

        switch (option) {
            case 'h':
                print_help_messages();
                exit(EXIT_SUCCESS);
            case 'a':
                options.absolute = true;
                break;
            case 'd':
                options.dirs_only = true;
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
        options.target = fs::path(argv[i]);
        break;
    }

    return options;
}

std::string sanitize_target(const std::optional<fs::path> &target_from_opts)
{
    fs::path target;

    if (target_from_opts.has_value()) {
        target = target_from_opts.value();
    } else {
        target = fs::current_path();
    }

    if (!fs::exists(target)) {
        throw std::runtime_error("Directory does not exist");
    }

    if (!fs::is_directory(target)) {
        throw std::runtime_error("Not a valid directory");
    }

    std::string target_str = target.string();
    strip_extra_path_delimiter(target_str);

    return target_str;
}

int main(int argc, char **argv)
{
    const Options options = parse_cli_options(argc, argv);
    const std::string target = sanitize_target(options.target);

    try {
        run_tree({ options.absolute, options.dirs_only, target });
    } catch (const fs::filesystem_error &e) {
        std::cerr << "Error: " << e.what() << '\n';
        return EXIT_FAILURE;
    } catch (const std::runtime_error &e) {
        std::cerr << "Error: " << e.what() << '\n';
        return EXIT_FAILURE;
    }

    return 0;
}
