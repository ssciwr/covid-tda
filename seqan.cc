#include <utility>
#include <vector>
 
#include <seqan3/alignment/pairwise/align_pairwise.hpp>
#include <seqan3/alignment/scoring/nucleotide_scoring_scheme.hpp>
#include <seqan3/alphabet/nucleotide/dna4.hpp>
#include <seqan3/core/debug_stream.hpp>
#include <seqan3/range/views/pairwise_combine.hpp>
 
using seqan3::operator""_dna4;
 
#include <utility>
#include <vector>
 
#include <seqan3/alignment/pairwise/align_pairwise.hpp>
#include <seqan3/alignment/scoring/nucleotide_scoring_scheme.hpp>
#include <seqan3/alphabet/nucleotide/dna4.hpp>
#include <seqan3/core/debug_stream.hpp>
#include <seqan3/range/views/pairwise_combine.hpp>
#include <seqan3/std/ranges>
 
using seqan3::operator""_dna4;
 
// This is taken from https://docs.seqan.de/seqan/3-master-user/cookbook.html#autotoc_md55

int main()
{
    std::vector vec{"ACGTGACTGACT"_dna4,
                    "ACGAAGACCGAT"_dna4,
                    "ACGTGACTGACT"_dna4,
                    "AGGTACGAGCGA"_dna4};
 
    // Configure the alignment kernel.
    auto config = seqan3::align_cfg::method_global{} |
                  seqan3::align_cfg::edit_scheme |
                  seqan3::align_cfg::min_score{-7} |
                  seqan3::align_cfg::output_score{};
 
    // auto filter_v = std::views::filter([](auto && res) { return res.score() >= -6;});
 
    for (auto const & res : seqan3::align_pairwise(seqan3::views::pairwise_combine(vec), config) | seqan3::views::persist)
    {
        seqan3::debug_stream << "Score: " << res.score() << '\n';
    }
}
