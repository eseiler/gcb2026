# SPDX-FileCopyrightText: 2006-2025 Knut Reinert & Freie Universität Berlin
# SPDX-FileCopyrightText: 2016-2025 Knut Reinert & MPI für molekulare Genetik
# SPDX-License-Identifier: CC0-1.0

cmake_minimum_required (VERSION 3.25)

# An object library (without main) to be used in multiple targets.
# You can add more external include paths of other projects that are needed for your project.
add_library (gcb20265_lib STATIC fastq_conversion.cpp)
target_include_directories (gcb20265_lib PUBLIC "${gcb20265_SOURCE_DIR}/include")
target_link_libraries (gcb20265_lib PUBLIC seqan3::seqan3 sharg::sharg seqan::hibf
                                                    fmindex-collection::fmindex-collection
)

target_compile_options (gcb20265_lib PUBLIC "-pedantic" "-Wall" "-Wextra")

option (gcb20265_WITH_WERROR "Report compiler warnings as errors." ON)

if (gcb20265_WITH_WERROR)
    target_compile_options (gcb20265_lib PUBLIC "-Werror")
    message (STATUS "Building tests with -Werror.")
endif ()

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    # Disable warning about std::hardware_destructive_interference_size not being ABI-stable.
    if (CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 12)
        target_compile_options (gcb20265_lib PUBLIC "-Wno-interference-size")
    endif ()

    # Warn about failed return value optimization.
    if (CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 14)
        target_compile_options (gcb20265_lib PUBLIC "-Wnrvo")
    endif ()
endif ()

# Add the application.
add_executable (gcb20265 main.cpp)
target_link_libraries (gcb20265 PRIVATE gcb20265_lib)
