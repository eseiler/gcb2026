// SPDX-FileCopyrightText: 2006-2026 Knut Reinert & Freie Universität Berlin
// SPDX-FileCopyrightText: 2016-2026 Knut Reinert & MPI für molekulare Genetik
// SPDX-License-Identifier: CC0-1.0

#include <iostream>
#include <filesystem>         // for std::filesystem(::path)
#include <fstream>            // for std::ofstream

#include <cereal/archives/binary.hpp>                     // for BinaryOutputArchive
#include <cereal/types/vector.hpp>                        // IWYU pragma: keep
#include <hibf/cereal/path.hpp>                           // IWYU pragma: keep
#include <hibf/hierarchical_interleaved_bloom_filter.hpp> // for hierarchical_interleaved_bloom_filter

#include <sharg/parser.hpp> // include the SeqAn sharg parser https://github.com/seqan/sharg-parsers

// argument parsing
struct cli_args
{
    std::filesystem::path input_path{};
    std::vector<std::filesystem::path> filenames;
};

/*!\brief Parses the command line arguments and returns a cli_args instance.
 * \param argc The argument count, as passed to `main`.
 * \param argv The argument vector, as passed to `main`.
 * \returns A `cli_args` object holding the parsed input directory and the list of
 *          filenames found directly inside it.
 * \details
 *
 * Sets up a sharg::parser exposing a single required option, `-i`/`--input`, which
 * names a directory. On success, `args.filenames` is populated by iterating that
 * directory (non-recursively) and collecting every entry's path.
 *
 * If parsing fails (e.g. the option is missing or the argument is invalid), the error
 * is printed to stderr and a default-constructed `cli_args` (empty `input_path` and
 * `filenames`) is returned instead of throwing.
 */
cli_args parse_cmd(int argc, char const * argv[])
{
    // "args" stores all command line parameters
    cli_args args{};

    // creates a sharg::parser instance called Papertracker,
    // hands over argc and argv for parsing
    // and disables update notifications (Doc: https://docs.seqan.de/sharg/main_user/about_update_notifications.html)
    sharg::parser parser{"Papertracker", argc, argv, sharg::update_notifications::off};

    // add CMD option '-i/--input' that sets the parameter args.input_path
    parser.add_option(
        args.input_path,
        sharg::config{.short_id = 'i', .long_id = "input", .description = "path to files", .required = true});

    try // go and let the parser parse the command line
    {
        parser.parse();
    }
    catch (sharg::parser_error const & ext)
    {
        std::cerr << "[COMMAND LINE INPUT ERROR] " << ext.what() << std::endl;
        return args;
    }

    // input path is now set.
    // loop over all files in args.input_path and store all filenames in args.filenames
    for (auto const & entry : std::filesystem::directory_iterator(args.input_path))
        args.filenames.push_back(entry.path());

    return args;
}

/*!\brief Serialises an HIBF together with its associated filenames to disk.
 * \param hibf The hierarchical_interleaved_bloom_filter to serialise.
 * \param filenames The filenames the HIBF's user bins were built from.
 * \param name The path of the file to write the archive to.
 * \details
 *
 * Opens a binary std::ofstream at `name` and writes `filenames` followed by `hibf`
 * via a cereal::BinaryOutputArchive. The same order (filenames, then hibf) must be
 * used when reading the archive back.
 */
void store(seqan::hibf::hierarchical_interleaved_bloom_filter const & hibf,
           std::vector<std::filesystem::path> const & filenames,
           std::filesystem::path const & name)
{
    std::ofstream fout{name, std::ios::binary};
    cereal::BinaryOutputArchive oarchive{fout};
    oarchive(filenames);
    oarchive(hibf);
}
