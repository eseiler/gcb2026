<!--
SPDX-FileCopyrightText: 2006-2025 Knut Reinert & Freie Universität Berlin
SPDX-FileCopyrightText: 2016-2025 Knut Reinert & MPI für molekulare Genetik
SPDX-License-Identifier: CC0-1.0
-->

# GCB Workshop 2026

## Benefits of an AMQ filter

### Task 1

Create a `task1.cpp` in `gcb2026/src/` and add the executable to `gcb2026/CMakeLists.txt`.

`task1.cpp` should:
* Parse the command line using `parse_cmd` to get the filenames of a input directory.
* Go over each filename,
   - check if the file contains all of the following three words: "3D","protein","structure"
   - if so, add the word count of the file to a total word counter
* Output the number of files that contain the three words and the total word count of those files.

### Task 2

Create a `task2.cpp` in `gcb2026/src/` and add the executable to `gcb2026/CMakeLists.txt`.

`task2.cpp` should:
* Parse the command line using `parse_cmd` to get the filenames of a input directory.
* Create an HIBF where each file is a user bin and its words are hashed with std::hash.
* Serialize the HIBF to disk using cereal (included in HIBF lib)
* Serialize the Filenames to disk using cereal

### Task 3

Create a `task3.cpp` in `gcb2026/src/` and add the executable to `gcb2026/CMakeLists.txt`.

`task3.cpp` should:
* Read in the HIBF and Filenames from task2
* Query the HIBF for the three words "3D","protein","structure"
* For each hit (user bin/file),
  - count the words in the file
  - double check that the three words are contained
  - if the three words are contained, add the word count of the file to the total word counter
* Output the number of files that contain the three words and the total word count of those files.