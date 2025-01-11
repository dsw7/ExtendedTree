#pragma once

#include <filesystem>
#include <optional>
#include <string>

struct Params {
    std::optional<std::string> level = std::nullopt;
    std::optional<std::filesystem::path> target = std::nullopt;
};

Params parse_cli(int argc, char **argv);
