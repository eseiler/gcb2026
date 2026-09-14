#include <iostream>   // for std::cout
#include <string>     // for std::string
#include <fstream>    // for fstream
#include <filesystem> // for std::filesystem(::path)

#include <chopper/parse_cmd.hpp> // local header that provides parse_cmd for command line parsing

#include <cereal/archives/binary.hpp> // for BinaryOutputArchive
#include <cereal/types/vector.hpp> // IWYU pragma: keep
#include <hibf/cereal/path.hpp> // IWYU pragma: keep

#include <hibf/config.hpp>                                // for config, insert_iterator
#include <hibf/hierarchical_interleaved_bloom_filter.hpp> // for hierarchical_interleaved_bloom_filter

// Build a Hierarchical Interleaved Bloom Filter (HIBF) on the Paper Data
int main(int argc, char const * argv[])
{
    cli_args args = parse_cmd(argc, argv);

    // The input lambda
    auto file_data = [&](size_t const user_bin_id, seqan::hibf::insert_iterator it)
    {
        std::fstream file{args.filenames[user_bin_id]};

        if (!file.is_open())
            throw std::runtime_error{"Could not open file"};

        std::string word;

        while (file >> word)
            it = std::hash<std::string>{}(word);
    };

    seqan::hibf::config config{.input_fn = file_data, // required
                               .number_of_user_bins = args.filenames.size(),     // required
                               .threads = 1u};

    // The HIBF constructor will determine a hierarchical layout for the user bins and build the filter.
    seqan::hibf::hierarchical_interleaved_bloom_filter hibf{config};

    std::ofstream fout{"test.hibf", std::ios::binary};
    cereal::BinaryOutputArchive oarchive{fout};
    oarchive(args.filenames);
    oarchive(hibf);
}