// SPDX-FileCopyrightText: 2006-2025 Knut Reinert & Freie Universität Berlin
// SPDX-FileCopyrightText: 2016-2025 Knut Reinert & MPI für molekulare Genetik
// SPDX-License-Identifier: CC0-1.0

#include <iostream>   // for std::cout
#include <string>     // for std::string
#include <fstream>    // for fstream
#include <filesystem> // for std::filesystem(::path)

#include <seqan3/io/sequence_file/input.hpp>
#include <seqan3/alignment/scoring/nucleotide_scoring_scheme.hpp>
#include <seqan3/alignment/pairwise/align_pairwise.hpp>

#include <solution_task4.hpp> // local header that provides parse_cmd for command line parsing

int main(int argc, char const * argv[])
{
    cli_args args = parse_cmd(argc, argv); // args contains filenames

    // read in query
    seqan3::sequence_file_input query_file{args.query_path};
    auto & query = (*query_file.begin()).sequence();

    for (std::filesystem::path filename : args.filenames)
    {
        seqan3::sequence_file_input reference_file{filename};

        for (auto && record : reference_file)
        {
            // Configure the alignment kernel.
            // Example of a semi-global alignment where leading and trailing gaps in the
            // second sequence are not penalised:
            auto config = seqan3::align_cfg::method_global{seqan3::align_cfg::free_end_gaps_sequence1_leading{true},
                                                        seqan3::align_cfg::free_end_gaps_sequence2_leading{false},
                                                        seqan3::align_cfg::free_end_gaps_sequence1_trailing{true},
                                                        seqan3::align_cfg::free_end_gaps_sequence2_trailing{false}} |
                        seqan3::align_cfg::scoring_scheme{seqan3::nucleotide_scoring_scheme{}};

            // Invoke the pairwise alignment which returns a lazy range over alignment results.
            auto results = seqan3::align_pairwise(std::tie(record.sequence(), query), config);
            auto & res = *results.begin(); // first and only alignment result

            std::cout << filenames[hit_user_bin] << ":" << record.id() << "\tscore:" << res.score() << std::endl;
        }
    }
}


