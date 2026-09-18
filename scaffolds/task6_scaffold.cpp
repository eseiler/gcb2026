// SPDX-FileCopyrightText: 2006-2026 Knut Reinert & Freie Universität Berlin
// SPDX-FileCopyrightText: 2016-2026 Knut Reinert & MPI für molekulare Genetik
// SPDX-License-Identifier: CC0-1.0

#include <fstream>            // for fstream
#include <iostream>           // for std::cout

#include <seqan3/alignment/pairwise/align_pairwise.hpp>
#include <seqan3/alignment/scoring/nucleotide_scoring_scheme.hpp>
#include <seqan3/io/sequence_file/input.hpp>
#include <seqan3/search/views/kmer_hash.hpp>

#include <hibf/config.hpp>                                // for config, insert_iterator
#include <hibf/hierarchical_interleaved_bloom_filter.hpp> // for hierarchical_interleaved_bloom_filter

#include <task6.hpp> // helper functions

int main(int argc, char const * argv[])
{
    cli_args args = parse_cmd(argc, argv);

    seqan::hibf::hierarchical_interleaved_bloom_filter hibf;
    std::vector<std::filesystem::path> filenames;

    store(XXX/*The HIBF objext*/, XXX/*The filenames object*/, XXX/*The index file name*/);

    XXX // create a query_file with a seqan3::sequence_file_input from the file path args.query_path
    auto & query = (*query_file.begin()).sequence(); // reads in first record

    auto query_hashes = query | XXX /*use seqan3 kmer_hash with a kmer size of 20*/);
    size_t const threshold = XXX /*Whats a good threshold?*/;

    auto agent = XXX/*get membership_agent from hibf*/;
    auto & result = agent.membership_for(XXX/*The query objext*/, XXX/*A threshold*/);

    std::cout << "There are " << result.size() << " hits" << std::endl;

    for (uint64_t hit_user_bin : result)
    {
        XXX // open a std::fstream on the file at position filenames[hit_user_bin]

        for (auto && record : reference_file)
        {
            // Configure the alignment kernel.
            // Example of a semi-global alignment where leading and trailing gaps in the
            // second sequence are not penalised:
            auto config = seqan3::align_cfg::method_global{seqan3::align_cfg::free_end_gaps_sequence1_leading{true},
                                                           seqan3::align_cfg::free_end_gaps_sequence2_leading{false},
                                                           seqan3::align_cfg::free_end_gaps_sequence1_trailing{true},
                                                           seqan3::align_cfg::free_end_gaps_sequence2_trailing{false}}
                        | seqan3::align_cfg::scoring_scheme{seqan3::nucleotide_scoring_scheme{}}
                        | seqan3::align_cfg::band_fixed_size{seqan3::align_cfg::lower_diagonal{-9000},
                                                             seqan3::align_cfg::upper_diagonal{500}};

            // Invoke the pairwise alignment which returns a lazy range over alignment results.
            auto results = seqan3::align_pairwise(std::tie(record.sequence(), query), config);
            auto & res = *results.begin(); // first and only alignment result

            XXX // print out filename, reference sequence name and score
        }
    }
}
