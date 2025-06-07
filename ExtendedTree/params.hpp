#pragma once

#include <set>
#include <string>

namespace params {

extern bool PRINT_HUMAN_READABLE;
extern bool PRINT_DIRS_ONLY;
extern bool PRINT_JSON;
extern int INDENT_LEVEL;
extern int LEVEL;
extern std::set<std::string> EXCLUDES;
extern std::string TARGET;

} // namespace params
