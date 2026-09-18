// SPDX-FileCopyrightText: 2006-2026 Knut Reinert & Freie Universität Berlin
// SPDX-FileCopyrightText: 2016-2026 Knut Reinert & MPI für molekulare Genetik
// SPDX-License-Identifier: CC0-1.0

#include <fstream>  // for fstream
#include <iostream> // for std::cout

#include <hibf/config.hpp>                                // for config, insert_iterator
#include <hibf/hierarchical_interleaved_bloom_filter.hpp> // for hierarchical_interleaved_bloom_filter

#include <task2.hpp> // helper functions

// Build a Hierarchical Interleaved Bloom Filter (HIBF) on the Paper Data
int main(int argc, char const * argv[])
{
    cli_args args = parse_cmd(argc, argv);

    // The input lambda
    auto file_data = [&](size_t const user_bin_id, seqan::hibf::insert_iterator it)
    {
        std::fstream file{args.filenames[user_bin_id]};

        std::string word;

        while (file >> word)
            it = std::hash<std::string>{}(word);
    };

    seqan::hibf::config config{.input_fn = file_data,                        // required
                               .number_of_user_bins = args.filenames.size(), // required
                               .threads = 1u};

    // The HIBF constructor will determine a hierarchical layout for the user bins and build the filter.
    seqan::hibf::hierarchical_interleaved_bloom_filter hibf{config};

    store(hibf, args.filenames, "hibf.index");
}
