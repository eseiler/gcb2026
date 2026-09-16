# SPDX-FileCopyrightText: 2006-2025 Knut Reinert & Freie Universität Berlin
# SPDX-FileCopyrightText: 2016-2025 Knut Reinert & MPI für molekulare Genetik
# SPDX-License-Identifier: CC0-1.0

cmake_minimum_required (VERSION 4.4)

# Enable `import std;` (C++23 standard library module). Requires the Ninja generator and Clang with libc++.
# These variables must be set before `project ()`.
# CMake's support is experimental; the UUID is specific to CMake 4.4 (see Help/dev/experimental.rst in CMake).
set (CMAKE_EXPERIMENTAL_CXX_IMPORT_STD "f35a9ac6-8463-4d38-8eec-5d6008153e7d")
cmake_diagnostic (SET CMD_EXPERIMENTAL IGNORE) # Silence the "support is experimental" warning.
# Clang provides the `std` module only through libc++, and warns that `std` is a reserved module name when
# compiling it, which -Werror would turn into an error.
set (CMAKE_CXX_FLAGS_INIT "-stdlib=libc++ -Wno-reserved-module-identifier")
# The `libc++.modules.json` that Clang reports by default (/usr/lib/x86_64-linux-gnu/) has broken relative
# paths (https://github.com/llvm/llvm-project/issues/172497), so name the intact one explicitly.
set (CMAKE_CXX_STDLIB_MODULES_JSON "/usr/lib/llvm-23/lib/libc++.modules.json")

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

# `ninja check` builds every executable defined in `src/CMakeLists.txt`, so you can
# quickly verify that everything (including the tasks you add) still compiles.
get_property (gcb20265_src_targets DIRECTORY src PROPERTY BUILDSYSTEM_TARGETS)
foreach (gcb20265_target IN LISTS gcb20265_src_targets)
    get_target_property (gcb20265_target_type ${gcb20265_target} TYPE)
    if (gcb20265_target_type STREQUAL "EXECUTABLE")
        list (APPEND gcb20265_check_targets ${gcb20265_target})
    endif ()
endforeach ()
add_custom_target (check DEPENDS ${gcb20265_check_targets})

