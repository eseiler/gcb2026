#include <iostream>   // for std::cout
#include <string>     // for std::string
#include <fstream>    // for fstream
#include <filesystem> // for std::filesystem(::path)

#include <parse_cmd.hpp> // local header that provides parse_cmd for command line parsing

#include <cereal/archives/binary.hpp> // for BinaryOutputArchive
#include <cereal/types/vector.hpp> // IWYU pragma: keep
#include <hibf/cereal/path.hpp> // IWYU pragma: keep

#include <hibf/config.hpp>                                // for config, insert_iterator
#include <hibf/hierarchical_interleaved_bloom_filter.hpp> // for hierarchical_interleaved_bloom_filter

int main()
{
    std::vector<std::filesystem::path> filenames;
    seqan::hibf::hierarchical_interleaved_bloom_filter hibf;
    std::ifstream fout{"test.hibf"};
    cereal::BinaryInputArchive iarchive{fout};
    iarchive(filenames);
    iarchive(hibf);

    std::vector<uint64_t> query{std::hash<std::string>{}("protein"),
                                std::hash<std::string>{}("3D"),
                                std::hash<std::string>{}("structure")};

    auto agent = hibf.membership_agent();
    auto & result = agent.membership_for(query, 3u);

    unsigned protein_structure_counter{};
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
            protein_structure_counter++;
            word_count += counter;
        }
    }
    std::cout << "DONE -- " << protein_structure_counter
              << "/" << hibf.number_of_user_bins
              << " files. Total of (words): " << word_count << std::endl;
}