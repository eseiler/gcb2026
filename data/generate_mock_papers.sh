#!/usr/bin/env bash
#
# SPDX-FileCopyrightText: 2006-2026 Knut Reinert & Freie Universität Berlin
# SPDX-FileCopyrightText: 2016-2026 Knut Reinert & MPI für molekulare Genetik
# SPDX-License-Identifier: CC0-1.0
#
# generate_test_files.sh - Generate random "lorem ipsum" style text files for testing.
#
# Usage:
#   ./generate_test_files.sh <file_word_count> <num_no_hit_files> <num_hits>
#
# Arguments:
#   file_word_count    Number of words in each generated file.
#   num_no_hit_files   Number of "no-hit" files to create (random text only).
#   num_hits           Number of "hit" files to create (random text + the
#                       three words "3D" "protein" "structure" appended).
#
# Output:
#   - No-hit files:  A0001.txt, A0002.txt, ... each containing exactly
#                     file_word_count random words.
#   - Hit files:      P0001.txt, P0002.txt, ... each containing
#                     (file_word_count - 3) random words followed by the
#                     words "3D", "protein" and "structure".
#
# Files are written to the current working directory.

set -euo pipefail

if [[ $# -ne 3 ]]; then
    echo "Usage: $0 <file_word_count> <num_no_hit_files> <num_hits>" >&2
    exit 1
fi

file_word_count=$1
num_no_hit_files=$2
num_hits=$3

if ! [[ "$file_word_count" =~ ^[0-9]+$ && "$num_no_hit_files" =~ ^[0-9]+$ && "$num_hits" =~ ^[0-9]+$ ]]; then
    echo "Error: all arguments must be non-negative integers." >&2
    exit 1
fi

if (( file_word_count < 3 )); then
    echo "Error: file_word_count must be at least 3 (hit files need room for the 3 appended words)." >&2
    exit 1
fi

# Word pool used to build random "lorem ipsum" style text.
words=(lorem ipsum dolor sit amet consectetur adipiscing elit sed do eiusmod
tempor incididunt ut labore et dolore magna aliqua enim ad minim veniam
quis nostrud exercitation ullamco laboris nisi aliquip ex ea commodo
consequat duis aute irure in reprehenderit voluptate velit esse cillum
eu fugiat nulla pariatur excepteur sint occaecat cupidatat non proident
sunt culpa qui officia deserunt mollit anim id est laborum)

num_words=${#words[@]}

# Print $1 random words (space-separated) from the pool.
random_words() {
    local count=$1
    local out=()
    local i
    for ((i = 0; i < count; i++)); do
        out+=("${words[$((RANDOM % num_words))]}")
    done
    (IFS=' '; echo "${out[*]}")
}

# Zero-pad a number to 4 digits, e.g. 7 -> 0007.
zero_pad() {
    printf "%04d" "$1"
}

echo "Generating $num_no_hit_files no-hit file(s) (A####.txt) with $file_word_count words each..."
for ((i = 1; i <= num_no_hit_files; i++)); do
    fname="A$(zero_pad "$i").txt"
    random_words "$file_word_count" > "$fname"
done

echo "Generating $num_hits hit file(s) (P####.txt) with $((file_word_count - 3)) random words + '3D protein structure'..."
for ((i = 1; i <= num_hits; i++)); do
    fname="P$(zero_pad "$i").txt"
    { random_words "$((file_word_count - 3))"; printf ' 3D protein structure'; echo; } > "$fname"
done

echo "Done."
