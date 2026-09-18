# SPDX-FileCopyrightText: 2006-2026 Knut Reinert & Freie Universität Berlin
# SPDX-FileCopyrightText: 2016-2026 Knut Reinert & MPI für molekulare Genetik
# SPDX-License-Identifier: CC0-1.0

cmake_minimum_required (VERSION 3.25)

# An object library (without main) to be used in multiple targets.
# You can add more external include paths of other projects that are needed for your project.
add_library (gcb2026_lib INTERFACE)
target_include_directories (gcb2026_lib INTERFACE "${gcb2026_SOURCE_DIR}/include")
target_link_libraries (gcb2026_lib INTERFACE seqan3::seqan3 sharg::sharg seqan::hibf
)

target_compile_options (gcb2026_lib INTERFACE "-pedantic" "-Wall" "-Wextra")

option (gcb2026_WITH_WERROR "Report compiler warnings as errors." ON)

if (gcb2026_WITH_WERROR)
    target_compile_options (gcb2026_lib INTERFACE "-Werror")
    message (STATUS "Building tests with -Werror.")
endif ()

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    # Disable warning about std::hardware_destructive_interference_size not being ABI-stable.
    if (CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 12)
        target_compile_options (gcb2026_lib INTERFACE "-Wno-interference-size")
    endif ()

    # Warn about failed return value optimization.
    if (CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 14)
        target_compile_options (gcb2026_lib INTERFACE "-Wnrvo")
    endif ()
endif ()

# Add the application.
#add_executable (task1 task1.cpp)
#target_link_libraries (task1 PRIVATE gcb2026_lib)
