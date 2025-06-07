#include "tree.hpp"

#include "params.hpp"
#include "utils.hpp"

#include <filesystem>
#include <fmt/core.h>
#include <iostream>
#include <optional>
#include <stdexcept>
#include <string>
#include <unistd.h>

namespace fs = std::filesystem;

void print_help_messages()
{
    fmt::print("Usage:\n  etree [OPTION]... DIRECTORY\n\n");
    fmt::print("Options:\n");
    fmt::print("  -a         {}\n", "Print absolute space usage (as opposed to relative usage)");
    fmt::print("  -b         {}\n", "Print absolute usage in bytes");
    fmt::print("  -d         {}\n", "Print directories only");
    fmt::print("  -j <level> {}\n", "Print output as JSON with indentation <level>");
    fmt::print("  -I         {}\n", "Exclude one or more files or directories");
    fmt::print("  -L <level> {}\n", "Descend <level> directories deep");
    fmt::print("  -h         {}\n", "Print this help message and exit");
}

std::string validate_target(const std::optional<fs::path> &target_from_opts)
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
    utils::strip_extra_path_delimiter(target_str);

    return target_str;
}

void parse_cli_options(int argc, char **argv)
{
    int option = 0;

    while ((option = getopt(argc, argv, "habdj:I:L:")) != -1) {
        switch (option) {
            case 'h':
                print_help_messages();
                exit(EXIT_SUCCESS);
            case 'a':
                params.print_absolute = true;
                break;
            case 'b':
                params.print_bytes = true;
                break;
            case 'd':
                params.print_dirs_only = true;
                break;
            case 'j':
                params.print_json = true;
                params.indent_level = std::atoi(optarg);
                break;
            case 'I':
                params.excludes.insert(optarg);
                break;
            case 'L':
                params.level = std::atoi(optarg);
                break;
            default:
                print_help_messages();
                exit(EXIT_FAILURE);
        }
    }

    std::optional<std::string> target = std::nullopt;

    for (int i = optind; i < argc; i++) {
        target = fs::path(argv[i]);
        break;
    }

    params.target = validate_target(target);
}

int main(int argc, char **argv)
{
    try {
        parse_cli_options(argc, argv);
        run_tree();
    } catch (const fs::filesystem_error &e) {
        std::cerr << "Error: " << e.what() << '\n';
        return EXIT_FAILURE;
    } catch (const std::runtime_error &e) {
        std::cerr << "Error: " << e.what() << '\n';
        return EXIT_FAILURE;
    }

    return 0;
}
