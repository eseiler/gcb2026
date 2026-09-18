// SPDX-FileCopyrightText: 2006-2026 Knut Reinert & Freie Universität Berlin
// SPDX-FileCopyrightText: 2016-2026 Knut Reinert & MPI für molekulare Genetik
// SPDX-License-Identifier: CC0-1.0

#include <filesystem>    // for std::filesystem(::path)
#include <fstream>       // for fstream
#include <iostream>      // for std::cout
#include <parse_cmd.hpp> // local header that provides parse_cmd for command line parsing
#include <string>        // for std::string

int main(int argc, char const * argv[])
{
    cli_args args = parse_cmd(argc, argv); // args contains filenames

    unsigned file_counter{};
    unsigned protein_structure_counter{};
    unsigned word_count{};

    for (std::filesystem::path filename : args.filenames)
    {
        std::fstream file{filename};

        std::string word;
        unsigned counter{0};
        bool text_protein{false};
        bool text_3D{false};
        bool text_structure{false};

        while (file >> word)
        {
            ++counter;
            if (word == "protein")
                text_protein = true;
            if (word == "3D")
                text_3D = true;
            if (word == "structure")
                text_structure = true;
        }

        if (text_protein && text_3D && text_structure)
        {
            protein_structure_counter++;
            word_count += counter;
        }

        ++file_counter;
    }

    std::cout << "DONE -- " << protein_structure_counter << "/" << file_counter << " files. Total of (words) "
              << word_count << std::endl;
}
