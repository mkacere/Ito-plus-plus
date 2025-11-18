cmake_minimum_required(VERSION 3.20)

project(ItoPlusPlus LANGUAGES CXX)

# Build type
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    message(STATUS "Setting build type to 'Release' as none was specified.")
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build." FORCE)
endif()

# ============================================================================
# Find TBB (required for std::execution::par on GCC/Clang)
# ============================================================================
find_package(TBB QUIET)

if(TBB_FOUND)
    message(STATUS "TBB found - parallel execution enabled")
else()
    message(STATUS "TBB not found - parallel execution may not work on GCC/Clang")
    message(STATUS "Install with: sudo apt install libtbb-dev  (Ubuntu/Debian)")
    message(STATUS "           or: brew install tbb             (macOS)")
endif()

# ============================================================================
# Header-only library target
# ============================================================================
add_library(ito_engine INTERFACE)

target_include_directories(ito_engine
    INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_compile_features(ito_engine INTERFACE cxx_std_23)

# Link TBB if found
if(TBB_FOUND)
    target_link_libraries(ito_engine INTERFACE TBB::tbb)
endif()

# ============================================================================
# COMPILER OPTIMIZATIONS
# ============================================================================
if(MSVC)
    target_compile_options(ito_engine INTERFACE
        /W4 /permissive- /Zc:__cplusplus
        $<$<CONFIG:Release>:/O2>
        $<$<CONFIG:Release>:/Ob2>
        $<$<CONFIG:Release>:/Oi>
        $<$<CONFIG:Release>:/Ot>
        $<$<CONFIG:Release>:/GL>
        $<$<CONFIG:Release>:/fp:fast>
    )
    
    target_link_options(ito_engine INTERFACE
        $<$<CONFIG:Release>:/LTCG>
        $<$<CONFIG:Release>:/OPT:REF>
        $<$<CONFIG:Release>:/OPT:ICF>
    )
else()
    target_compile_options(ito_engine INTERFACE
        -Wall -Wextra -Wpedantic
        $<$<CONFIG:Release>:-O3>
        $<$<CONFIG:Release>:-march=native>
        $<$<CONFIG:Release>:-mtune=native>
        $<$<CONFIG:Release>:-ffast-math>
        $<$<CONFIG:Release>:-funroll-loops>
        $<$<CONFIG:Release>:-flto>
        $<$<CONFIG:Release>:-DNDEBUG>
    )
    
    target_link_options(ito_engine INTERFACE
        $<$<CONFIG:Release>:-flto>
    )
endif()

add_subdirectory(demos)