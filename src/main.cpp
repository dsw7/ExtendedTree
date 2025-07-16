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
    const std::string messages = R"(-- ExtendedTree | Copyright (C) 2025 by David Weber

A program for recursively calculating relative disk usage. Program
returns information similar to "ncdu" but in like tree-like format.

Usage:
  etree [OPTIONS] [DIRECTORY]

Options:
  -b        Print sizes in bytes
  -d        Print directories only
  -j LEVEL  Print output as JSON with indentation LEVEL
  -I        Exclude one or more files or directories
  -L LEVEL  Descend LEVEL directories deep
  -h        Print this help message and exit
)";

    fmt::print("{}", messages);
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

    while ((option = getopt(argc, argv, "hbdj:I:L:")) != -1) {
        switch (option) {
            case 'h':
                print_help_messages();
                exit(EXIT_SUCCESS);
            case 'b':
                params::PRINT_BYTES = true;
                break;
            case 'd':
                params::PRINT_DIRS_ONLY = true;
                break;
            case 'j':
                params::PRINT_JSON = true;
                params::INDENT_LEVEL = std::atoi(optarg);
                break;
            case 'I':
                params::EXCLUDES.insert(optarg);
                break;
            case 'L':
                params::LEVEL = std::atoi(optarg);
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

    params::TARGET = validate_target(target);
}

int main(int argc, char **argv)
{
    try {
        parse_cli_options(argc, argv);
        tree::run_tree();
    } catch (const fs::filesystem_error &e) {
        std::cerr << "Error: " << e.what() << '\n';
        return EXIT_FAILURE;
    } catch (const std::runtime_error &e) {
        std::cerr << "Error: " << e.what() << '\n';
        return EXIT_FAILURE;
    }

    return 0;
}
