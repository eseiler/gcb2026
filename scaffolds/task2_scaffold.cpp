// SPDX-FileCopyrightText: 2006-2026 Knut Reinert & Freie Universität Berlin
// SPDX-FileCopyrightText: 2016-2026 Knut Reinert & MPI für molekulare Genetik
// SPDX-License-Identifier: CC0-1.0

#include <fstream>  // for fstream
#include <iostream> // for std::cout

#include <hibf/config.hpp>                                // for config, insert_iterator
#include <hibf/hierarchical_interleaved_bloom_filter.hpp> // for hierarchical_interleaved_bloom_filter

#include <task2.hpp> // helper functions

// Task2: Build a Hierarchical Interleaved Bloom Filter (HIBF) on the mock paper data
int main(int argc, char const * argv[])
{
    cli_args args = parse_cmd(argc, argv);

    // The input lambda. A function that takes to input parameters: file_idx and it
    // file_idx = the current index of the file that is processed by the HIBF
    // it = an iterator that gets the input hashes
    auto file_data = [&](size_t const file_idx, seqan::hibf::insert_iterator it)
    {
        XXX // open a std::fstream on the file at position file_idx in args.filenames

        std::string word;

        while (file >> word)
            it = XXX /*Use std::hash to hash the word*/;
    };

    seqan::hibf::config config{.input_fn = XXX /* Hand over the file data */,
                               .number_of_user_bins = XXX /*supply the number of files*/};

    // The HIBF constructor will determine a hierarchical layout for the user bins and build the filter.
    seqan::hibf::hierarchical_interleaved_bloom_filter hibf{XXX};

    // store the HIBF on disk
    store(XXX/*The HIBF objext*/, XXX/*The filenames object*/, XXX/*A file name*/);
}
