# SPDX-FileCopyrightText: 2006-2026 Knut Reinert & Freie Universität Berlin
# SPDX-FileCopyrightText: 2016-2026 Knut Reinert & MPI für molekulare Genetik
# SPDX-License-Identifier: CC0-1.0

cmake_minimum_required (VERSION 3.25)

# Define the application name and version.
project (gcb20265
         LANGUAGES CXX
         VERSION 1.0.0
         DESCRIPTION "My application description"
)

# This allows including `*.cmake` files from the `cmake` directory without specifying the full path.
list (APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")

# Sets C++ standard and link time optimisation.
include (cxx_config)

# Specify the directories where to store the built archives, libraries and executables.
include (output_directories)

# Add packages.
# We use CPM for package management: https://github.com/cpm-cmake/CPM.cmake
# The added packages (e.g., hibf, sharg, seqan3) are defined in the `cmake/package-lock.cmake` file.
include (CPM)
CPMUsePackageLock (${CMAKE_CURRENT_LIST_DIR}/cmake/package-lock.cmake)

CPMGetPackage (use_ccache) # Speeds up compilation by caching files that have been compiled before.

# Comment/Uncomment entries as needed.
# If you change something here, make sure to add/remove the target in target_link_libraries of src/CMakeLists.txt.
# CPMGetPackage (fmindex) # FM-Index
CPMGetPackage (hibf) # (Hierarchical) Interleaved Bloom Filter
CPMGetPackage (seqan3) # Sequence analysis
CPMGetPackage (sharg) # Argument parser

# Add the application. This will include `src/CMakeLists.txt`.
add_subdirectory (src)

# `make check` builds every executable defined in `src/CMakeLists.txt`, so you can
# quickly verify that everything (including the tasks you add) still compiles.
get_property (gcb20265_src_targets DIRECTORY src PROPERTY BUILDSYSTEM_TARGETS)
foreach (gcb20265_target IN LISTS gcb20265_src_targets)
    get_target_property (gcb20265_target_type ${gcb20265_target} TYPE)
    if (gcb20265_target_type STREQUAL "EXECUTABLE")
        list (APPEND gcb20265_check_targets ${gcb20265_target})
    endif ()
endforeach ()
add_custom_target (check DEPENDS ${gcb20265_check_targets})

