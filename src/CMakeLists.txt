cmake_minimum_required(VERSION 3.18)
project(ExtendedTree VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_BUILD_TYPE RelWithDebInfo)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
add_compile_options(-Wall -Wextra -pedantic -Werror)

# -----------------------------------------------------------------------------------------------------------
# {fmt}

message(STATUS "Locating {fmt} library")
find_package(fmt)

if(NOT fmt_FOUND)
  message(FATAL_ERROR "Is the {fmt} package installed?")
endif()

# -----------------------------------------------------------------------------------------------------------
# nlohmann/json.hpp

message(STATUS "Checking for nlohmann/json.hpp")

set(NLOHMANN_JSON_HPP "${CMAKE_SOURCE_DIR}/external/json.hpp")

if(NOT EXISTS ${NLOHMANN_JSON_HPP})
  message(STATUS "Downloading nlohmann/json.hpp")
  file(
    DOWNLOAD
    "https://raw.githubusercontent.com/nlohmann/json/develop/single_include/nlohmann/json.hpp"
    ${NLOHMANN_JSON_HPP}
    SHOW_PROGRESS
  )
else()
  message(STATUS "nlohmann/json.hpp already exists. Skipping download")
endif()

# -----------------------------------------------------------------------------------------------------------

include_directories(external)
add_executable(
  etree
  filenode.cpp
  main.cpp
  params.cpp
  reporting.cpp
  tree.cpp
  utils.cpp
)
target_link_libraries(etree fmt::fmt)

set(CMAKE_INSTALL_PREFIX "$ENV{HOME}/.local")
install(TARGETS etree DESTINATION bin)
