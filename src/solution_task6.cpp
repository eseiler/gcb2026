// SPDX-FileCopyrightText: 2006-2025 Knut Reinert & Freie Universität Berlin
// SPDX-FileCopyrightText: 2016-2025 Knut Reinert & MPI für molekulare Genetik
// SPDX-License-Identifier: CC0-1.0

#include <iostream>   // for std::cout
#include <string>     // for std::string
#include <fstream>    // for fstream
#include <filesystem> // for std::filesystem(::path)

#include <cereal/archives/binary.hpp> // for BinaryOutputArchive
#include <cereal/types/vector.hpp> // IWYU pragma: keep
#include <hibf/cereal/path.hpp> // IWYU pragma: keep

#include <hibf/config.hpp>                                // for config, insert_iterator
#include <hibf/hierarchical_interleaved_bloom_filter.hpp> // for hierarchical_interleaved_bloom_filter

#include <seqan3/io/sequence_file/input.hpp>
#include <seqan3/alignment/scoring/nucleotide_scoring_scheme.hpp>
#include <seqan3/alignment/pairwise/align_pairwise.hpp>
#include <seqan3/search/views/kmer_hash.hpp>

#include <solution_task4.hpp> // local header that provides parse_cmd for command line parsing

int main(int argc, char const * argv[])
{
    std::vector<std::filesystem::path> filenames;
    seqan::hibf::hierarchical_interleaved_bloom_filter hibf;
    std::ifstream fout{"test.hibf"};
    cereal::BinaryInputArchive iarchive{fout};
    iarchive(filenames);
    iarchive(hibf);

    cli_args args = parse_cmd(argc, argv);

    seqan3::sequence_file_input query_file{args.query_path};
    auto & query = (*query_file.begin()).sequence();
    auto query_hashes = query | seqan3::views::kmer_hash(seqan3::ungapped{20});

    auto agent = hibf.membership_agent();
    auto & result = agent.membership_for(query_hashes, 3u);

    for (uint64_t hit_user_bin : result)
    {
        seqan3::sequence_file_input reference_file{filenames[hit_user_bin]};

        for (auto && record : reference_file)
        {
            // Configure the alignment kernel.
            // Example of a semi-global alignment where leading and trailing gaps in the
            // second sequence are not penalised:
            auto config = seqan3::align_cfg::method_global{seqan3::align_cfg::free_end_gaps_sequence1_leading{false},
                                                        seqan3::align_cfg::free_end_gaps_sequence2_leading{true},
                                                        seqan3::align_cfg::free_end_gaps_sequence1_trailing{false},
                                                        seqan3::align_cfg::free_end_gaps_sequence2_trailing{true}} |
                        seqan3::align_cfg::scoring_scheme{seqan3::nucleotide_scoring_scheme{}};

            // Invoke the pairwise alignment which returns a lazy range over alignment results.
            auto results = seqan3::align_pairwise(std::tie(record.sequence(), query), config);
            auto & res = *results.begin(); // first and only alignment result

            if (res.score() > 80)
                std::cout << filenames[hit_user_bin] << std::endl;
        }
    }
}