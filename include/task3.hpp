// SPDX-FileCopyrightText: 2006-2026 Knut Reinert & Freie Universität Berlin
// SPDX-FileCopyrightText: 2016-2026 Knut Reinert & MPI für molekulare Genetik
// SPDX-License-Identifier: CC0-1.0

#include <iostream>
#include <filesystem>         // for std::filesystem(::path)
#include <fstream>            // for std::ifstream

#include <cereal/archives/binary.hpp>                     // for BinaryInputArchive
#include <cereal/types/vector.hpp>                        // IWYU pragma: keep
#include <hibf/cereal/path.hpp>                           // IWYU pragma: keep
#include <hibf/hierarchical_interleaved_bloom_filter.hpp> // for hierarchical_interleaved_bloom_filter

#include <sharg/parser.hpp> // include the SeqAn sharg parser https://github.com/seqan/sharg-parsers

// argument parsing
struct cli_args
{
    std::filesystem::path index_path{};
};

/*!\brief Parses the command line arguments and returns a cli_args instance.
 * \param argc The argument count, as passed to `main`.
 * \param argv The argument vector, as passed to `main`.
 * \returns A `cli_args` object holding the parsed path of the HIBF index file.
 * \details
 *
 * Sets up a sharg::parser exposing a single required option, `-i`/`--index`, which
 * names the HIBF index file to read (see `load()`).
 *
 * If parsing fails (e.g. the option is missing or the argument is invalid), the error
 * is printed to stderr and a default-constructed `cli_args` (empty `index_path`) is
 * returned instead of throwing.
 */
cli_args parse_cmd(int argc, char const * argv[])
{
    // "args" stores all command line parameters
    cli_args args{};

    // creates a sharg::parser instance called Papertracker,
    // hands over argc and argv for parsing
    // and disables update notifications (Doc: https://docs.seqan.de/sharg/main_user/about_update_notifications.html)
    sharg::parser parser{"Papertracker", argc, argv, sharg::update_notifications::off};

    // add CMD option '-i/--index' that sets the parameter args.index_path
    parser.add_option(
        args.index_path,
        sharg::config{.short_id = 'i',
                      .long_id = "index",
                      .description = "path to the HIBF index file to load",
                      .required = true});

    try // go and let the parser parse the command line
    {
        parser.parse();
    }
    catch (sharg::parser_error const & ext)
    {
        std::cerr << "[COMMAND LINE INPUT ERROR] " << ext.what() << std::endl;
        return args;
    }

    return args;
}

/*!\brief Deserialises an HIBF together with its associated filenames from disk.
 * \param hibf The hierarchical_interleaved_bloom_filter to deserialise into.
 * \param filenames The vector to deserialise the associated filenames into.
 * \param name The path of the file to read the archive from.
 * \details
 *
 * Opens a binary std::ifstream at `name` and reads `filenames` followed by `hibf`
 * via a cereal::BinaryInputArchive, mirroring the order written by `store()`.
 */
void load(seqan::hibf::hierarchical_interleaved_bloom_filter & hibf,
          std::vector<std::filesystem::path> & filenames,
          std::filesystem::path const & name)
{
    std::ifstream fin{name, std::ios::binary};
    cereal::BinaryInputArchive iarchive{fin};
    iarchive(filenames);
    iarchive(hibf);
}
