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
    cli_args args = parse_cmd(argc, argv); // args contains index_path

    // create objects that are filled when loaded
    seqan::hibf::hierarchical_interleaved_bloom_filter hibf;
    std::vector<std::filesystem::path> filenames;

    load(XXX/*The HIBF objext*/, XXX/*The filenames object*/, args.index_path);

    // The query contains the three words hashed with std::hash
    std::vector<uint64_t> query{std::hash<std::string>{}("protein"),
                                std::hash<std::string>{}("3D"),
                                std::hash<std::string>{}("structure")};

    auto agent = XXX/*get membership_agent from hibf*/;
    auto & result = agent.membership_for(XXX/*The query objext*/, XXX/*A threshold*/);

    for (uint64_t hit_user_bin : result)
    {
        XXX // open a std::fstream on the file at position filenames[hit_user_bin]

        if (!file.is_open())
        {
            std::cout << "Could not open file\n";
            return 1;
        }

        XXX // copy over your code from task 1 to count the words in each file
    }
}
