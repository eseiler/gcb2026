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
| [sharg](https://github.com/seqan/sharg-parsers) | Command-line argument parsing (already wired up for you in `include/parse_cmd.hpp`) | Python's `argparse` |
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
| `scaffolds/` | If you are unexperienced in C++ use the scaffolds to solve the tasks. |
| `solutions/` | Don't peak! |

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

If you haven't been coding the scaffold during the workshop with us copy over `scaffolds/task1_scaffold.cpp` to `src/task1.cpp`.

For each file, check if the **file containes the words** `"3D"`, `"protein"` and `"structure"` and if so add the word count of the file to a total word counter. In the end print out
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

**Starting point:** copy `src/gcb_task2_scaffold.cpp` to `src/task2.cpp`.

`task2.cpp` should:
1. Get `args.filenames` via `parse_cmd`.
2. Write an `input_fn` lambda with the signature
   `[&](size_t const user_bin_id, seqan::hibf::insert_iterator it)`. Inside
   it, open `args.filenames[user_bin_id]`, read the file word by word, and
   for each word do `it = std::hash<std::string>{}(word);` — this inserts
   the word's hash into that file's user bin.
3. Build a `seqan::hibf::config` with:
   - `.input_fn = <your lambda>` (required),
   - `.number_of_user_bins = args.filenames.size()` (required),
   - `.threads = 1u`.
4. Construct the filter: `seqan::hibf::hierarchical_interleaved_bloom_filter hibf{config};`.
5. Serialize **both** `hibf` and `args.filenames` to disk with a
   `cereal::BinaryOutputArchive` (you need the filenames again in task 3 to
   turn a matched user-bin index back into a path).

Needed includes: `<cereal/archives/binary.hpp>`, `<hibf/config.hpp>`,
`<hibf/hierarchical_interleaved_bloom_filter.hpp>`.

## Task 3 — Query the index, then verify only the candidates

**Starting point:** copy `src/gcb_task3_scaffold.cpp` to `src/task3.cpp`.

`task3.cpp` should:
1. Load the `hierarchical_interleaved_bloom_filter` and the filenames
   vector back from `test.hibf` with a `cereal::BinaryInputArchive`
   (mirroring how task 2 saved them).
2. Build a query: `std::vector<uint64_t>` holding
   `std::hash<std::string>{}(word)` for each of `"protein"`, `"3D"`,
   `"structure"`.
3. Get a `hibf.membership_agent()` and call
   `agent.membership_for(query, 3u)` — the `3u` means "only report user
   bins where all 3 of the query hashes were found". The result is a range
   of user-bin indices, i.e. candidate files.
4. For each candidate index, look up its filename in the filenames vector,
   open **only that file**, and re-check (exactly as in task 1) that all
   three words are really present — the filter can have false positives,
   so this confirmation step is required.
5. If confirmed, add the file's word count to the running total.
6. Print the same summary as task 1: number of confirmed files (out of the
   total number of user bins) and their total word count.

Compare how many files you actually had to open here versus in task 1 —
that gap is the benefit an AMQ filter gives you.

---

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

**Starting point:** copy your `task1.cpp` to `task4.cpp`.

First, create a new header `include/mapper_parse_cmd.hpp` by copying
`include/parse_cmd.hpp` and extending its `cli_args`/`parse_cmd`:
1. Add a member `std::filesystem::path query_path;` to `cli_args`.
2. Add a new required sharg option `-q`/`--query` that fills
   `args.query_path` (copy the pattern used for `-i`/`--input`).

(`include/solution_task4.hpp` is the reference solution for this header, in
case you want to check your work.)

Then in `task4.cpp`:
1. Include `<seqan3/io/sequence_file/input.hpp>`.
2. Read the query sequence:
   `seqan3::sequence_file_input query_file{args.query_path};` and take the
   `.sequence()` of its first (and only) record — see the
   [`sequence_file_input` docs](https://docs.seqan.de/seqan3/main_user/classseqan3_1_1sequence__file__input.html).
3. Replace `std::fstream` with `seqan3::sequence_file_input` for each
   reference file.
4. Replace the `while (file >> word)` loop with a `for (auto && record :
   reference_file)` loop over the FASTA/FASTQ records.
5. For each reference record, align `record.sequence()` against the query
   with `seqan3::align_pairwise` (use a semi-global configuration, i.e.
   leading/trailing gaps in the query are free — see the
   [pairwise alignment tutorial](https://docs.seqan.de/seqan3/main_user/tutorial_pairwise_alignment.html)).
4. For each alignment print the reference filename, the reference name (`record.id()`), and the alignment score (`res.score()`) to `std::cout` for every candidate (see "Interpreting alignment scores" below for how to judge them by eye).

## Task 5 — Build an AMQ index over reference k-mers

**Starting point:** copy your `task2.cpp` to `task5.cpp`.

1. Replace `std::fstream` with `seqan3::sequence_file_input` for the
   reference files (include `<seqan3/io/sequence_file/input.hpp>`).
2. Replace the word-reading `while` loop with a `for` loop over the
   FASTA/FASTQ records.
3. For each record, hash its sequence into k-mers with
   `record.sequence() | seqan3::views::kmer_hash(seqan3::ungapped{k})`
   (include `<seqan3/search/views/kmer_hash.hpp>`; try `k = 20` to start,
   then feel free to experiment) and insert every hash via `it = hash`,
   just like you inserted word hashes in task 2. See the
   [`kmer_hash` docs](https://docs.seqan.de/seqan3/main_user/group__search__views.html#ga6e598d6a021868f704d39df73252974f).

## Task 6 — Query the index, then align only the candidates

**Starting point:** copy your `task3.cpp` to `task6.cpp`.

1. Build the query from the query file instead of from three literal
   words: read `args.query_path` with `seqan3::sequence_file_input` (as in
   task 4) and hash its sequence into k-mers with the same
   `seqan3::views::kmer_hash(seqan3::ungapped{k})` you used in task 5 (same
   `k`!).
2. Replace `std::fstream` with `seqan3::sequence_file_input` for the
   reference files, and the word-reading loop with a `for` loop over
   records, as in task 4.
3. Call `agent.membership_for(query_hashes, threshold)` as in task 3 —
   pick a `threshold` that makes sense for the number of k-mers your query
   produces (a query with hundreds of k-mers behaves very differently from
   one with three words; experiment and see how it affects your results).
4. For each candidate user bin, open the reference file and, for every
   record, compute the semi-global alignment against the query exactly as
   in task 4. Again, print the reference filename, `record.id()`, and `res.score()` to `std::cout` for every candidate (see "Interpreting alignment scores" below for how to judge them by eye).

---

# Step by step task solving

## Task 1

If you haven't been coding the scaffold during the workshop with us copy over `scaffolds/task1_scaffold.cpp` to `src/task1.cpp`.

1. **Before the for loop**, create three counter variables of type `unsinged` initialized to `0`:  `file_counter`, `paper_counter`, `total_word_count`, like this `unsigned count{0};`
2. **Before the while loop**, create a counter variable of type `unsigned` initialized to `0` named `word_counter`.
3. **Before the while loop**, create three `bool`eans named `text_protein`, `text_3D`, `text_structure` initialized to `false`.
4. **Within the for loop** increase `word_counter` by one, since we can count the words while checking them right away.
5. **Within the for loop** create an if clause for each query word (`"3D"`, `"protein"` and `"structure"`) checking if the variable `word` equals the query word and if so, set the respective boolean to true (e.g. `if *(word == "foo) text_foo = true`).
6. **After the while loop** Check with an if clause if all three booleans from step 3 are set to true and if so, increase `paper_counter ` by one and add `word_counter` to `total_word_count`.
7. **After the while loop** increase `file_counter` by one.
8. **After the for loop** print out all counters using `std::cout`, e.g. << `std::cout << "done: " << counter;`.

Then run your program:

```
./build/task1 -i data/mock_papers
```
Tip: `src/task1.py` implements the exact same logic in Python — useful to
compare against once your C++ version compiles and runs.

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

Caveats: `N`/ambiguous IUPAC bases score as hard mismatches, not neutral, so
assembly gaps inflate the apparent divergence.

