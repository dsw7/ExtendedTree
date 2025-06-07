#include "report_json.hpp"

#include "params.hpp"
#include "utils.hpp"

#include <fmt/core.h>
#include <json.hpp>

namespace {

nlohmann::json build_json_from_tree(const std::unique_ptr<FileNode> &node)
{
    nlohmann::json j;

    j["filename"] = node->filename;

    if (node->is_file()) {
        j["filesize"] = node->get_filesize();
    } else if (node->is_directory()) {
        j["filesize"] = node->get_filesize();
    } else {
        j["filesize"] = nullptr;
    }

    j["children"] = nlohmann::json::array();

    for (const auto &child: node->children) {
        j["children"].push_back(build_json_from_tree(child));
    }

    return j;
}

nlohmann::json build_json_from_tree(const std::unique_ptr<FileNode> &node, uintmax_t total_size)
{
    nlohmann::json j;

    j["filename"] = node->filename;

    if (node->is_file()) {
        j["filesize"] = utils::compute_relative_usage(node->get_filesize(), total_size);
    } else if (node->is_directory()) {
        j["filesize"] = utils::compute_relative_usage(node->get_filesize(), total_size);
    } else {
        j["filesize"] = nullptr;
    }

    j["children"] = nlohmann::json::array();

    for (const auto &child: node->children) {
        j["children"].push_back(build_json_from_tree(child, total_size));
    }

    return j;
}

} // namespace

namespace reporting {

void print_json(const std::unique_ptr<FileNode> &node)
{
    nlohmann::json json = build_json_from_tree(node);
    fmt::print("{}\n", json.dump(params.indent_level));
}

void print_json(const std::unique_ptr<FileNode> &node, uintmax_t total_size)
{
    nlohmann::json json = build_json_from_tree(node, total_size);
    fmt::print("{}\n", json.dump(params.indent_level));
}

} // namespace reporting
