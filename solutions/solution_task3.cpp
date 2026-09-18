// SPDX-FileCopyrightText: 2006-2026 Knut Reinert & Freie Universität Berlin
// SPDX-FileCopyrightText: 2016-2026 Knut Reinert & MPI für molekulare Genetik
// SPDX-License-Identifier: CC0-1.0

#include <fstream>       // for fstream
#include <iostream>      // for std::cout

#include <hibf/config.hpp>                                // for config, insert_iterator
#include <hibf/hierarchical_interleaved_bloom_filter.hpp> // for hierarchical_interleaved_bloom_filter

#include <task3.hpp> // helper functions

int main(int argc, char const * argv[])
{
    cli_args args = parse_cmd(argc, argv);

    seqan::hibf::hierarchical_interleaved_bloom_filter hibf;
    std::vector<std::filesystem::path> filenames;

    load(hibf, filenames, args.index_path);

    std::vector<uint64_t> query{std::hash<std::string>{}("protein"),
                                std::hash<std::string>{}("3D"),
                                std::hash<std::string>{}("structure")};

    auto agent = hibf.membership_agent();
    auto & result = agent.membership_for(query, 3u);

    unsigned paper_counter{};
    unsigned word_count{};
    for (uint64_t hit_user_bin : result)
    {
        unsigned counter{};

        std::fstream file{filenames[hit_user_bin]};

        if (!file.is_open())
        {
            std::cout << "Could not open file\n";
            return 1;
        }

        std::string word;

        bool text_protein{false};
        bool text_3D{false};
        bool text_structure{false};
        while (file >> word)
        {
            counter++;
            if (word == "protein")
                text_protein = true;
            if (word == "3D")
                text_3D = true;
            if (word == "structure")
                text_structure = true;
        }

        if (text_protein && text_3D && text_structure)
        {
            paper_counter++;
            word_count += counter;
        }
    }
    std::cout << "DONE -- " << paper_counter << "/" << hibf.number_of_user_bins
              << " files. Total of (words): " << word_count << std::endl;
}
