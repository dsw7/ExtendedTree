cmake_minimum_required(VERSION 3.18)
project(ExtendedTree VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_BUILD_TYPE RelWithDebInfo)

string(TIMESTAMP BUILD_DATE)
message(STATUS "Will add the following timestamp to binary: ${BUILD_DATE}")

add_compile_definitions(PROJECT_NAME="${PROJECT_NAME}")
add_compile_definitions(PROJECT_VERSION="${PROJECT_VERSION}")
add_compile_definitions(BUILD_DATE="${BUILD_DATE}")

add_compile_options(-Wall -Wextra -pedantic -Werror)

include_directories(
  "external"
  "include"
)
add_executable(
  "etree"
  "src/main.cpp"
)

set(CMAKE_INSTALL_PREFIX "$ENV{HOME}/.local")
install(TARGETS "etree" DESTINATION "bin")
