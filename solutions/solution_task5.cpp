// SPDX-FileCopyrightText: 2006-2026 Knut Reinert & Freie Universität Berlin
// SPDX-FileCopyrightText: 2016-2026 Knut Reinert & MPI für molekulare Genetik
// SPDX-License-Identifier: CC0-1.0

#include <iostream>      // for std::cout

#include <seqan3/io/sequence_file/input.hpp>
#include <seqan3/search/views/kmer_hash.hpp>

#include <hibf/config.hpp>                                // for config, insert_iterator
#include <hibf/hierarchical_interleaved_bloom_filter.hpp> // for hierarchical_interleaved_bloom_filter

#include <task5.hpp> // helper functions

// Build a Hierarchical Interleaved Bloom Filter (HIBF) on the RefSeq data
int main(int argc, char const * argv[])
{
    cli_args args = parse_cmd(argc, argv);

    // The input lambda
    auto file_data = [&](size_t const file_idx, seqan::hibf::insert_iterator it)
    {
        seqan3::sequence_file_input file{args.filenames[file_idx]};

        for (auto && record : file)
        {
            auto hashes = record.sequence() | seqan3::views::kmer_hash(seqan3::ungapped{20});
            for (uint64_t hash : hashes)
                it = hash;
        }
    };

    seqan::hibf::config config{.input_fn = file_data,
                               .number_of_user_bins = args.filenames.size()};

    // The HIBF constructor will determine a hierarchical layout for the user bins and build the filter.
    seqan::hibf::hierarchical_interleaved_bloom_filter hibf{config};

    store(hibf, args.filenames, args.index_path);
}
