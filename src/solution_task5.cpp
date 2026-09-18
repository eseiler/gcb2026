// SPDX-FileCopyrightText: 2006-2026 Knut Reinert & Freie Universität Berlin
// SPDX-FileCopyrightText: 2016-2026 Knut Reinert & MPI für molekulare Genetik
// SPDX-License-Identifier: CC0-1.0

#include <filesystem>    // for std::filesystem(::path)
#include <fstream>       // for fstream
#include <iostream>      // for std::cout
#include <parse_cmd.hpp> // local header that provides parse_cmd for command line parsing
#include <string>        // for std::string

#include <seqan3/io/sequence_file/input.hpp>
#include <seqan3/search/views/minimiser_hash.hpp>

#include <cereal/archives/binary.hpp>                     // for BinaryOutputArchive
#include <cereal/types/vector.hpp>                        // IWYU pragma: keep
#include <hibf/cereal/path.hpp>                           // IWYU pragma: keep
#include <hibf/config.hpp>                                // for config, insert_iterator
#include <hibf/hierarchical_interleaved_bloom_filter.hpp> // for hierarchical_interleaved_bloom_filter

// Build a Hierarchical Interleaved Bloom Filter (HIBF) on the Paper Data
int main(int argc, char const * argv[])
{
    cli_args args = parse_cmd(argc, argv);

    // The input lambda
    auto file_data = [&](size_t const user_bin_id, seqan::hibf::insert_iterator it)
    {
        seqan3::sequence_file_input file{args.filenames[user_bin_id]};

        for (auto && record : file)
        {
            // for (uint64_t hash : record.sequence()
            //                          | seqan3::views::minimiser_hash(seqan3::shape{seqan3::ungapped{20}},
            //                                                          seqan3::window_size{20},
            //                                                          seqan3::seed{0}))
            for (uint64_t hash : record.sequence() | seqan3::views::kmer_hash(seqan3::ungapped{20}))
                it = hash;
        }
    };

    seqan::hibf::config config{.input_fn = file_data,                        // required
                               .number_of_user_bins = args.filenames.size(), // required
                               .threads = 1u};

    // The HIBF constructor will determine a hierarchical layout for the user bins and build the filter.
    seqan::hibf::hierarchical_interleaved_bloom_filter hibf{config};

    std::ofstream fout{"test.hibf", std::ios::binary};
    cereal::BinaryOutputArchive oarchive{fout};
    oarchive(args.filenames);
    oarchive(hibf);
}
