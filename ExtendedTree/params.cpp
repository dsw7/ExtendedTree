#include "params.hpp"

#include <fmt/core.h>
#include <unistd.h>

Params parse_cli(int argc, char **argv)
{
    Params params;
    int option = 0;

    while ((option = getopt(argc, argv, "L:")) != -1) {
        switch (option) {
            case 'L':
                params.level = optarg;
                break;
            default:
                fmt::print(stderr, "Error parsing command line\n");
                exit(EXIT_FAILURE);
        }
    }

    for (int i = optind; i < argc; i++) {
        params.target = std::filesystem::path(argv[i]);
        break;
    }

    return params;
}
