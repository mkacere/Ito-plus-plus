cmake_minimum_required(VERSION 3.20)

# Use a target-safe project name (avoid '+' in target names)
project(ItoPlusPlus LANGUAGES CXX)

# Header-only library target that consumers can link to
add_library(ito_engine INTERFACE)

# Public headers live in ./include
target_include_directories(ito_engine
    INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Require modern C++
target_compile_features(ito_engine INTERFACE cxx_std_23)

# Nice warnings (optional but recommended)
if (MSVC)
    target_compile_options(ito_engine INTERFACE /W4 /permissive- /Zc:__cplusplus)
else()
    target_compile_options(ito_engine INTERFACE -Wall -Wextra -Wpedantic -Wconversion)
endif()

# Demos (executables that showcase the library)
add_subdirectory(demos)