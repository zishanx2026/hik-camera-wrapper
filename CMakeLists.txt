cmake_minimum_required(VERSION 3.10)

project(camera
    VERSION 1.0
    DESCRIPTION "Camera project"
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_subdirectory(lib)
add_subdirectory(app)