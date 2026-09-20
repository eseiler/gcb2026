// SPDX-FileCopyrightText: 2006-2025 Knut Reinert & Freie Universität Berlin
// SPDX-FileCopyrightText: 2016-2025 Knut Reinert & MPI für molekulare Genetik
// SPDX-License-Identifier: CC0-1.0

#include <fstream>
#include <print>

#include <task1.hpp> // includes helper functions

int main(int argc, char const * argv[])
{
    cli_args args = parse_cmd(argc, argv); // args contains filenames

    unsigned file_counter{};     // counts all files
    unsigned paper_counter{};    // counts files that contain 3D,protein,structure
    unsigned total_word_count{}; // counts the words for files that contain 3D,protein,structure

    for (std::filesystem::path filename : args.filenames)
    {
        std::fstream file{filename};

        if (!file.is_open())
        {
            std::println("Could not open file");
            return 1;
        }

        unsigned word_counter{0};
        bool text_protein{false};
        bool text_3D{false};
        bool text_structure{false};

        std::string word;

        while (file >> word)
        {
            ++word_counter;
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
            total_word_count += word_counter;
        }

        ++file_counter;
    }

    std::println("DONE -- {}/{} files. Total of (words) {}", paper_counter, file_counter, total_word_count);
}
