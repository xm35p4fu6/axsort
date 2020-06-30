#pragma once
#include <type_traits>
#include <numeric>

namespace AXSORT :: string_sorter {

    // assertion: open all assert() if true. useful for debug.
    // map2idx(): map the original sequence's character to bucket's index
    // map2val(): reverse of map2idx()
    template<typename SEQ_, typename RES_>
    class config 
    {
        using SEQ = typename std::remove_reference<SEQ_>::type;
        using RES = typename std::remove_reference<RES_>::type;
      public:
        using res_idx_t = typename RES::size_type;
        using res_val_t = typename RES::value_type;
        using seq_idx_t = typename SEQ::size_type;
        using seq_val_t = typename SEQ::value_type;
        using bkt_idx_t = seq_val_t;

        inline bkt_idx_t map2idx(seq_val_t v) { return v; }
        inline seq_val_t map2val(bkt_idx_t v) { return v; }

        static constexpr bool assertion = true;
        static constexpr auto empty =
            std::numeric_limits<res_val_t>::max();

        bkt_idx_t alphabet_size = 
            std::numeric_limits<seq_val_t>::max();
        seq_idx_t sequence_length = 0;
    };

}
