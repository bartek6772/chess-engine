cmake_minimum_required(VERSION 3.28)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(chess)

add_subdirectory(src/engine)
add_subdirectory(src/cli)
add_subdirectory(src/gui)
add_subdirectory(tests/)

# Forces the final executable to be in the root of /build
# set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})