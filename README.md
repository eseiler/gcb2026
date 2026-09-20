<!--
SPDX-FileCopyrightText: 2006-2026 Knut Reinert & Freie Universität Berlin
SPDX-FileCopyrightText: 2016-2026 Knut Reinert & MPI für molekulare Genetik
SPDX-License-Identifier: CC0-1.0
-->

# GCB Workshop 2026 — Approximate Membership Queries (AMQ)

This workshop is about **Approximate Membership Query (AMQ) filters**: data
structures that answer "could this item be in that set?" using much less
memory and time than checking the set directly. They can
give **false positives** ("maybe in the set" when it isn't) but never
**false negatives** (if the item is really in the set, the filter will never
say no). That one-sided error is what makes them useful as a fast
pre-filter: you use the AMQ to throw out everything that is *definitely
irrelevant*, and only run your slow, exact check (reading a whole file,
aligning a sequence, ...) on the small number of candidates it leaves
behind.

The specific AMQ filter we use here is the **HIBF** (Hierarchical
Interleaved Bloom Filter), built on top of the classic Bloom filter. It can
index many "user bins" (e.g. one bin per file, or per genome) at once and
answer "which of these bins might contain hash X?" very quickly.

## Libraries you'll touch

You don't need prior experience with any of these — every task below tells
you exactly which calls to use.

| Library | What it's for | Rough analogy |
|---|---|---|
| [sharg](https://github.com/seqan/sharg-parsers) | Command-line argument parsing (already wired up for you in each task's `include/taskN.hpp`) | Python's `argparse` |
| [seqan3](https://docs.seqan.de/seqan3/main_user/) | Reading FASTA/FASTQ files, computing k-mer hashes, pairwise sequence alignment | Biopython |
| [HIBF](https://github.com/seqan/hibf) | The AMQ filter (Hierarchical Interleaved Bloom Filter) itself | — |
| [cereal](https://uscilab.github.io/cereal/) | Serializing C++ objects to/from a binary file, so you can build an index once and query it later | Python's `pickle` |

A couple of C++ idioms you'll see that may look unfamiliar coming from
another language:
* `[&](size_t id, insert_iterator it) { ... }` is a **lambda**, an inline
  anonymous function. `[&]` means "capture surrounding variables by
  reference", similar to a Python closure.
* `sequence | seqan3::views::kmer_hash(seqan3::ungapped{20})` is a
  **range pipeline**: `sequence` is lazily transformed into a range of
  k-mer hashes, similar to chaining `.map()`/generator expressions.

## Repository tour

| Path | What it is |
|---|---|
| `include/` | Each task has a helper `.hpp` file you can include. |
| `src/` | Put source files here that you will use during this workshop. |
| `scaffolds/` | If you are inexperienced in C++ use the scaffolds to solve the tasks. |
| `solutions/` | Don't peek! |

## Build & run

```sh
mkdir build && cd build
cmake ..
make task1   # build a single task, once you've added it (see below)
```

Executables land directly in `build/`, so once built you run them as:

```sh
./build/task1 -i /path/to/input/dir
```

### Adding your own task as a build target

Each task needs its own line pair in `src/CMakeLists.txt` (**not** the
top-level `CMakeLists.txt`). Copy the pattern already used for the
solutions, e.g. for `task1.cpp`:

```cmake
add_executable (task1 task1.cpp)
target_link_libraries (task1 PRIVATE gcb2026_lib)
```

Re-run `cmake .` in the build directory once after adding a new executable so CMake picks it up.

---

# Part 1 — Benefits of an AMQ filter

We have a directory of plain-text "papers". We want to find every paper
that mentions all three words `"3D"`, `"protein"`, `"structure"`, and the
total word count across those papers.

* **Task 1** does this the naive way: open and read *every* file.
* **Task 2** builds an HIBF index over all files (one file = one user bin).
* **Task 3** queries that index first, and only opens the (usually much
  smaller) set of files the filter flagged as candidates — this is the
  speed-up an AMQ filter buys you.

## Task 1 — Baseline: scan every file

If you haven't been coding the scaffold during the workshop with us, copy over `scaffolds/task1_scaffold.cpp` to `src/task1.cpp`.

For each file, check if the **file contains the words** `"3D"`, `"protein"` and `"structure"` and if so add the word count of the file to a total word counter. In the end print out
- the total number of files,
- the number of files that contained the three query words and
- the total word count of all files that contained the three query words

Then run your program:

```
./build/task1 -i data/mock_papers
```

Tip: `src/task1.py` implements the exact same logic in Python — useful to
compare against once your C++ version compiles and runs.

**Need more guidance?** Check out the step by step guide at the end

## Task 2 — Build an AMQ index over all files

Write the file `task2.cpp` that uses the helper functions in `task2.hpp` for parsing the command line (`parse_cmd`) and storing a hibf (`store`).

Create an HIBF on the same files as before, supplying only required fields to the HIBF config object. The input to the HIBF is a lambda function, that should open a file and hash each word in the file using `std::hash`. In the end, store the index using the `store` function from `task2.hpp` and use the parameter `args.index_path` parsed from the command line as the output filename.

Documentation you'll need:
- HIBF lib: https://github.com/seqan/hibf (note the snippet on the landing page)
- HIBF API documentation: https://docs.seqan.de/hibf/main/index.html
- `std::hash`: https://en.cppreference.com/cpp/utility/hash

**Need more guidance?** Check out the step by step guide at the end

## Task 3 — Query the index, then verify only the candidates

Write the file `task3.cpp` that uses the helper functions in `task3.hpp` for parsing the command line (`parse_cmd`) and loading a hibf (`load`).

Parse the command line with `parse_cmd`. Create an HIBF and filenames object and load them from disk given the `args.index_path` path. Create a vector of three query hashes, that contain the words `"3D"`, `"protein"` and `"structure"` hashed by `std::hash`. Use the HIBF to query these words in the index with an appropriate threshold. Loop over the resulting hits and do the same as in task 1: Open the candidate file, count the words, if all three query words are contained, add the count to the total word counter and output the results in the end.

Documentation you'll need:
- [HIBF lib on GitHub](https://github.com/seqan/hibf) (note the snippet on the landing page)
- [HIBF API documentation](https://docs.seqan.de/hibf/main/index.html)
- [`std::hash`](https://en.cppreference.com/cpp/utility/hash)

**Need more guidance?** Check out the step by step guide at the end

## Benchmarking session 1

Report your timings

| Executable | data set      | time (sec) |
|------------|---------------|------------|
| task1.cpp  | mock paper    |            |
| task2.cpp  | mock paper    |            |
| task3.cpp  | mock paper    |            |
|    <hr>    |     <hr>      |   <hr>     |
| task1.cpp  | mock paper 2  |            |
| task2.cpp  | mock paper 2  |            |
| task3.cpp  | mock paper 2  |            |

---

Discuss the timings with regard to the efficiency of an Approximate Membership Query data structure as a prefilter.

# Part 2 — Read mapper with AMQ

Same idea as Part 1, but on real sequence data (FASTA/FASTQ) instead of
plain text, and using **k-mer hashes** instead of whole-word hashes. A
k-mer is just a substring of fixed length `k` taken from a sequence; hashing
all k-mers of a sequence is the standard way to represent it for a Bloom
filter. Instead of an exact string match, "confirmation" here means running
a real pairwise **semi-global alignment** and checking its score.

Tasks 4–6 mirror tasks 1–3 one-to-one: task 4 is the naive baseline, task 5
builds the AMQ index, task 6 queries it and only aligns the candidates.

## Task 4 — Baseline: align the query against every reference

In `task4.cpp` using `task4.hpp` helper functions `parse_cmd` you should parse the command line with `parse_cmd` that now has, additionally to the `-i/--input` option, a `-q/--query` option for the input query path. Create `seqan3::sequence_file_input` on the query filepath and store the sequence from the first and only record of the file in a variable `query`. Loop over all files and open each reference file using `seqan3::sequence_file_input` again. Loop over each record in the sequence file (there can be plasmid DNA additional to the genome). For each reference sequence compute a semi global alignment using `seqan3::align_pairwise` with a `seqan3::align_cfg` config object that configures `seqan3::align_cfg::method_global` for a semi global alignment, `seqan3::align_cfg::scoring_scheme` with a `seqan3::nucleotide_scoring_scheme{}` and `seqan3::align_cfg::band_fixed_size` with `seqan3::align_cfg::lower_diagonal{-9000}` and `seqan3::align_cfg::upper_diagonal{500}`. Print the filename, reference name and alignment score for each file to the command line.

Documentation you'll need:
- [seqan3 sequence file](https://docs.seqan.de/seqan3/main_user/classseqan3_1_1sequence__file__input.html)
- [seqan3 pairwise alignment](https://docs.seqan.de/seqan3/main_user/tutorial_pairwise_alignment.html)

**Need more guidance?** Check out the step by step guide at the end

## Task 5 — Build an AMQ index over reference k-mers

Write the file `task5.cpp` that uses the helper functions in `task5.hpp` for parsing the command line (`parse_cmd`) and storing a hibf (`store`).

Parse the command line as usual. Create a `file_data` lambda function as you have done in task 2 but now open a `seqan3::sequence_file_input` for the current file, loop over the records (of reference sequences) in the file and then loop over each record's `sequence()`, hashed by the `seqan3::views::kmer_hash`. Construct the `seqan3::views::kmer_hash` with a k-mer size of `20` and assign each hash to the iterator `it` of the lambda. Construct and store the hibf as you have done in task 2.

Documentation you'll need:
- [seqan3 sequence file](https://docs.seqan.de/seqan3/main_user/classseqan3_1_1sequence__file__input.html)
- [seqan3::views::kmer_hash](https://docs.seqan.de/seqan3/main_user/group__search__views.html#ga6e598d6a021868f704d39df73252974f)
- [HIBF API documentation](https://docs.seqan.de/hibf/main/index.html)

**Need more guidance?** Check out the step by step guide at the end

## Task 6 — Query the index, then align only the candidates

Write the file `task6.cpp` that uses the helper functions in `task6.hpp` for parsing the command line (`parse_cmd`) and loading a hibf (`load`).

After parsing the command line, load the hibf and filenames from disk as you have done in task 3. Then read in the query sequence as you have done in task 4. Hash the query using the `seqan3::views::kmer_hash` with a kmer size of `20`. Query the HIBF as you have done in task 3, this time providing the hashed query and a threshold that reflects that `90%` of the hashes match. Loop over the resulting hits as you have done in task 3 and compute a semi global alignment between each reference file and the query sequence as you have done in task 4.

Documentation you'll need:
- [seqan3 sequence file](https://docs.seqan.de/seqan3/main_user/classseqan3_1_1sequence__file__input.html)
- [seqan3::views::kmer_hash](https://docs.seqan.de/seqan3/main_user/group__search__views.html#ga6e598d6a021868f704d39df73252974f)
- [HIBF API documentation](https://docs.seqan.de/hibf/main/index.html)
- [seqan3 pairwise alignment](https://docs.seqan.de/seqan3/main_user/tutorial_pairwise_alignment.html)

**Need more guidance?** Check out the step by step guide at the end

## Benchmarking session 2

Report your timings

| Executable | data set      | time (sec) |
|------------|---------------|------------|
| task4.cpp  | RefSeq        |            |
| task5.cpp  | RefSeq        |            |
| task6.cpp  | RefSeq        |            |

---

# Step by step task solving

## Task 1

If you haven't been coding the scaffold during the workshop with us, copy over `scaffolds/task1_scaffold.cpp` to `src/task1.cpp`.

1. **Before the for loop**, create three counter variables of type `unsigned` initialized to `0`: `file_counter`, `paper_counter`, `total_word_count`, like this `unsigned count{0};`
2. **Before the while loop**, create a counter variable of type `unsigned` initialized to `0` named `word_counter`.
3. **Before the while loop**, create three `bool`eans named `text_protein`, `text_3D`, `text_structure` initialized to `false`.
4. **Within the while loop**, increase `word_counter` by one, since we can count the words while checking them right away.
5. **Within the while loop**, create an if clause for each query word (`"3D"`, `"protein"` and `"structure"`) checking if the variable `word` equals the query word and if so, set the respective boolean to true (e.g. `if (word == "foo") text_foo = true;`).
6. **After the while loop**, check with an if clause if all three booleans from step 3 are set to true and if so, increase `paper_counter` by one and add `word_counter` to `total_word_count`.
7. **After the while loop**, increase `file_counter` by one.
8. **After the for loop**, print out all counters using `std::println`, e.g. `std::println("done: {}/{} files, total words: {}", paper_counter, file_counter, total_word_count);`. Each `{}` in the format string is replaced by the next argument.

Then run your program:

```
./build/task1 -i data/mock_papers
```
Tip: `src/task1.py` implements the exact same logic in Python — useful to
compare against once your C++ version compiles and runs.

## Task 2

Building an HIBF on the mock paper data

1. Create `src/task2.cpp` and add it to the `src/CMakeLists.txt` file as an additional executable.
2. Create a `main` function like in task 1.
3. Include `#include <fstream>`, `#include <print>` and `#include <task2.hpp>`.
4. Parse the command line just as in task 1 (using `parse_cmd`). If you take a look in `task2.hpp` you can see that it now has two options
   - `-i/--input` for the input directory
   - `-o/--output-index` for the index filename
5. Copy over this lambda signature
   ```c++
   auto file_data = [&](size_t const file_idx, seqan::hibf::insert_iterator it)
   {
       // todo
   };
   ```
6. Within the `file_data` lambda
   1. Open a `std::fstream` named `file` that is constructed with the filename at position `file_idx` in the vector `args.filenames`. A vector is accessed via `[]`.
   2. Create a `std::string` named `word`.
   3. Create a while loop just as in task 1 that loops over the words in `file` and for each word assigns its hash to the iterator like this: `it = std::hash<std::string>{}(word)`.
7. Add includes: `<hibf/config.hpp>` and
`<hibf/hierarchical_interleaved_bloom_filter.hpp>`.
8. Create a `seqan::hibf::config` config object named `config` that is initialized using designated initialisers. Set `.input_fn` to `file_data` and `.number_of_user_bins` to the number of files (hint: the size of a vector `v` can be accessed with `v.size()`).
9. Construct the filter: `seqan::hibf::hierarchical_interleaved_bloom_filter hibf` initialized with the config object from step 8.
10. Store **both** `hibf` and `args.filenames` to disk using the store function `store(hibf, args.filenames, args.index_path)`.

Need more help yet? Use the scaffold `scaffolds/task2_scaffold.cpp`.

## Task 3

Querying an HIBF on the mock paper data and counting words

1. Create `src/task3.cpp` and add it to the `src/CMakeLists.txt` file as an additional executable.
2. Create a `main` function like in task 1.
3. Include `#include <fstream>`, `#include <print>` and `#include <task3.hpp>`.
4. Parse the command line just as in task 1 (using `parse_cmd`). If you take a look in `task3.hpp` you can see that it has the only option `-i/--index` for the index filename.
5. Create a `seqan::hibf::hierarchical_interleaved_bloom_filter` named `hibf` without initialising it. As an example, creating a number without initialising it is done by `int a;`.
6. Create `std::vector<std::filesystem::path>` named filenames without initialising it.
7. Load hibf index and filenames using the load function `load(hibf, filenames, args.index_path)`.
8. Construct a `std::vector<uint64_t>` on three input arguments: `"3D"`, `"protein"` and `"structure"` each hashed by `std::hash` just as in task 2. A vector with three elements is for example constructed like this: `std::vector<uint64_t>{1,2,3}`.
9. Create a hibf agent called `agent` using the member function `membership_agent()` on the `hibf` object. The type of the `agent` variable can be `auto`. Example with `auto`: `auto a = 3`, a is type `int`.
10. Use the `agent` to query the HIBF by using the member function `agent.membership_for(query, threshold)`, passing the query vector and an appropriate numeric threshold value (e.g. `1` or `1000`). The `result` of the member function is again stored in a variable using `auto`.
11. The result is a vector of numeric values indicating the file indices of files that likely contain the three query words. Loop over the `result` vector with a for loop `for (file_idx : result)`.
12. Now you need to do the exact same analysis as you have done in task 1 for the candidate files. Follow the steps 1-8 of task 1 again or copy over the code and adapt it to this for loop.

Need more help yet? Use the scaffold `scaffolds/task3_scaffold.cpp`.

## Task 4

1. Create `src/task4.cpp` and add it to the `src/CMakeLists.txt` file as an additional executable.
2. Create a `main` function like in task 1.
3. Include `#include <print>` and `#include <task4.hpp>`.
4. Parse the command line just as in task 1 (using `parse_cmd`). If you take a look in `task4.hpp` you can see that it now has two options
   - `-i/--input` for the input directory
   - `-q/--query` for the query filename
5. Include `<seqan3/io/sequence_file/input.hpp>`.
6. Construct a `seqan3::sequence_file_input` file named `query_file` given the query path `args.query_path`. ([`sequence_file_input` docs](https://docs.seqan.de/seqan3/main_user/classseqan3_1_1sequence__file__input.html))
7. Retrieve the query sequence of the first and only record like this: `auto & query = (*query_file.begin()).sequence()`. Explanation: `query_file.begin()` gives an iterator to the query_file range. `*` dereferences the iterator, giving you the first record, and `sequence()` gives you the sequence of the record.
8. Loop over the filenames in `args.filenames` as you have done in task 1.
9. Within the for loop, construct a `seqan3::sequence_file_input` named `reference_file` on the current filename.
10. Loop over the records of the `reference_file` via `for (auto && record : reference_file)`.
11. Within this for loop, create the following alignment config object,
  but replace each XXX with either true or false to configure a correct semi-global
  alignment:
  ```c++
              auto config = seqan3::align_cfg::method_global{seqan3::align_cfg::free_end_gaps_sequence1_leading{XXX},
                                                           seqan3::align_cfg::free_end_gaps_sequence2_leading{XXX},
                                                           seqan3::align_cfg::free_end_gaps_sequence1_trailing{XXX},
                                                           seqan3::align_cfg::free_end_gaps_sequence2_trailing{XXX}}
                        | seqan3::align_cfg::scoring_scheme{seqan3::nucleotide_scoring_scheme{}}
                        | seqan3::align_cfg::band_fixed_size{seqan3::align_cfg::lower_diagonal{-9000},
                                                             seqan3::align_cfg::upper_diagonal{500}};
  ```
12. Then, invoke the pairwise alignment which returns a lazy range over alignment results with `auto result_range = seqan3::align_pairwise(std::tie(record.sequence(), query), config)`.
13. Given the `result_range` range, acquire its start iterator with the member function `begin()` and directly dereference this iterator using `*` to get the `alignment_result` of type `auto &`.
14. Print out the current `filename`, the reference name via the member function `id()` of the `record` and the score of the alignment via `alignment_result.score()`, e.g. `std::println("{}:{}\tscore:{}", filename.string(), record.id(), alignment_result.score());`. Note that a `std::filesystem::path` cannot be printed directly with `std::println`; convert it with `.string()` first.

Need more help yet? Use the scaffold `scaffolds/task4_scaffold.cpp`.

## Task 5

1. Create `src/task5.cpp` and add it to the `src/CMakeLists.txt` file as an additional executable.
2. Create a `main` function like in task 1.
3. Include `#include <print>` and `#include <task5.hpp>`.
4. Parse the command line just as in task 1 (using `parse_cmd`). If you take a look in `task5.hpp` you can see that it now has two options
   - `-i/--input` for the input directory
   - `-o/--output-index` for the index filename
5. Include `<seqan3/io/sequence_file/input.hpp>`.
6. Copy over this lambda signature
   ```c++
   auto file_data = [&](size_t const file_idx, seqan::hibf::insert_iterator it)
   {
       // todo
   };
   ```
7. Within the lambda, construct a `seqan3::sequence_file_input` named `file` on the current filename.
8. Loop over the `record`s of the `file` as you have for example done in task 4.
9. Within this loop create the `hashes` of type `auto` by piping `|` the `seqan3::views::kmer_hash` onto the record's sequence (`record.sequence()`). Construct the `seqan3::views::kmer_hash` with a `seqan3::ungapped` shape that represents a standard kmer of size `20`.
10. Loop over the `hashes` view and add each hash to the lambda iterator via `it = hash`.
11. After the `file_data` lambda construct and store the hibf just as in task 2. Follow task 2 steps 7-10.

Need more help yet? Use the scaffold `scaffolds/task5_scaffold.cpp`.

## Task 6

1. Create `src/task6.cpp` and add it to the `src/CMakeLists.txt` file as an additional executable.
2. Create a `main` function like in task 1.
3. Include `#include <print>` and `#include <task6.hpp>`.
4. Parse the command line just as in task 1 (using `parse_cmd`). If you take a look in `task6.hpp` you can see that it now has two options
   - `-i/--index` for the index filename
   - `-q/--query` for the query filename
5. Follow task 3 steps 5-7 to load the `hibf` index and `filenames`.
6. Follow task 4 steps 5-7 to read the `query` sequence from file.
7. Create a `query_hashes` range of type `auto` by piping (`|`) the `seqan3::views::kmer_hash` onto the `query` sequence. Construct the `kmer_hash` with a `seqan3::ungapped` shape initialised to a size of `20`.
8. Query the HIBF and loop over the result by following task 3 steps 9-11.
9. Compute a semi-global alignment for each reference sequence by following task 4 steps 9-14.

Need more help yet? Use the scaffold `scaffolds/task6_scaffold.cpp`.

## Interpreting alignment scores (rough guide)

With the default `seqan3::nucleotide_scoring_scheme{}` (match `0`, mismatch
`-1`) and the default linear gap cost (`-1` per gap character), `score` is
exactly the negative edit distance between the query and its best-matching
window in the reference: `score = -(mismatches + indel_bases)`, max `0`.

| score range | approx. identity | interpretation |
|---|---|---|
| ≥ −400 | ≥95% | strong/confident hit |
| −1600 to −400 | 80–95% | good, likely true positive |
| −3200 to −1600 | 60–80% | marginal, scrutinize |
| < −3200 | <60% | noise floor (random background ≈ −0.75·L_query) |

