#include "params.hpp"

#include <fmt/core.h>
#include <unistd.h>

Params parse_cli(int argc, char **argv)
{
    Params p;

    int c = 0;
    while ((c = getopt(argc, argv, "L:")) != -1) {
        switch (c) {
            case 'L':
                p.level = optarg;
                break;
            default:
                fmt::print(stderr, "Error parsing command line\n");
                exit(EXIT_FAILURE);
        }
    }

    for (int i = optind; i < argc; i++) {
        p.target = std::filesystem::path(argv[i]);
        break;
    }

    return p;
}
