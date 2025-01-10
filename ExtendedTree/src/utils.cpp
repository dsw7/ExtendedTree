#include "utils.hpp"

#include <iostream>

void print(const std::string &message)
{
    std::cout << message << '\n';
}

void print_error(const std::string &message)
{
    std::cerr << message << '\n';
}
