// SPDX-FileCopyrightText: 2006-2025 Knut Reinert & Freie Universität Berlin
// SPDX-FileCopyrightText: 2016-2025 Knut Reinert & MPI für molekulare Genetik
// SPDX-License-Identifier: CC0-1.0

#include <iostream>
#include <fstream>

#include <task1.hpp> // includes helper functions

int main(int argc, char const * argv[])
{
    cli_args args = parse_cmd(argc, argv); // args contains filenames (args.filenames)

    for (std::filesystem::path filename : args.filenames)
    {
        std::fstream file{filename};

        if (!file.is_open())
        {
            std::cout << "Could not open file\n";
            return 1;
        }

        std::string word;

        while (file >> word)
        {
            // XXX task 1
        }
    }

    std::cout << "DONE " << std::endl;
}