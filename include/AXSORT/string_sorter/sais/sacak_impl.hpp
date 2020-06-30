#pragma once
#include <numeric>
#include <vector>
#include <array>
#include <limits>
#include <span>
#include <ranges>
#include <algorithm>
#include "../config.hpp"
#include "sais_impl.hpp"

namespace AXSORT :: string_sorter :: sacak_impl {

const bool show_function = false;

template<typename SEQ, typename RES, typename CFG_>
void saca_k(SEQ&&, RES&&, CFG_&&);

template <typename SEQ, typename RES, typename BKT
    , typename ISL, typename CFG>
inline void put_LMSc(SEQ&&, RES&&, BKT&&, ISL&&, CFG&&);






template<bool Level1 = 1, typename SEQ, typename RES, typename CFG_>
void saca_k(SEQ&& seq, RES&& res, CFG_&& cfg) 
{
    using CFG = typename std::remove_reference<CFG_>::type;
    using idx_t = typename CFG::res_idx_t;

    idx_t len(cfg.sequence_length);
    idx_t insert_pos(0), rank(0);

    std::vector<idx_t> bkt_cnt, bkt_ptr;
    if constexpr (Level1 == true)
    {
        bkt_cnt.resize(cfg.alphabet_size);
        bkt_ptr.resize(cfg.alphabet_size);
    }

    std::cerr << "sais(len=" << len << ",  alphabet=" << cfg.alphabet_size << ")\n";

    sais_impl::sequence_check(seq, cfg);

    if constexpr (Level1 == true)
    {
        sais_impl::cal_bucket_info(seq, bkt_cnt, cfg);
    }

    std::fill(res.begin(), res.begin()+len, CFG::empty);

    if constexpr (Level1 == true)
    {
        sais_impl::cal_bucket_ptr<forward>(bkt_ptr, bkt_cnt, cfg);
    }
    put_LMSc<Level1>(seq, res, bkt_ptr, cfg);

    if constexpr (Level1 == true)
    {
        sais_impl::cal_bucket_ptr<backward>(bkt_ptr, bkt_cnt, cfg);
    }
    induce_sort<induceL, LMS>(res, seq, is_L, bkt_ptr, cfg);

    if constexpr (Level1 == true)
    {
        sais_impl::cal_bucket_ptr<forward>(bkt_ptr, bkt_cnt, cfg);
    }
    induce_sort<induceS, LMS>(res, seq, is_L, bkt_ptr, cfg);
    res[0] = len-1;     // sentinel

    collect_element<forward>(res, insert_pos, cfg);
    cal_new_str(res, seq, is_L, insert_pos, rank);
    collect_element<backward>(res, insert_pos, cfg);

    if(++rank < insert_pos)
    {   
        recursive_call_sais(res, is_L, insert_pos, rank, cfg);
    }

    // clear bucket
    std::fill(res.begin()+insert_pos, res.begin()+len, CFG::empty);
    if constexpr (Level1 == true)
    {
        sais_impl::cal_bucket_ptr<forward>(bkt_ptr, bkt_cnt, cfg);
    }

    put_LMS_sorted(res, seq, bkt_ptr, insert_pos, cfg);
    if constexpr (Level1 == true)
    {
        sais_impl::cal_bucket_ptr<backward>(bkt_ptr, bkt_cnt, cfg);
    }
    induce_sort<induceL, suffix>(res, seq, is_L, bkt_ptr, cfg);

    if constexpr (Level1 == true)
    {
        sais_impl::cal_bucket_ptr<forward>(bkt_ptr, bkt_cnt, cfg);
    }
    induce_sort<induceS, suffix>(res, seq, is_L, bkt_ptr, cfg);
}

template <
    bool Level1
  , typename SEQ, typename RES, typename BKT, typename CFG >
inline void put_LMSc(
        SEQ&& seq, RES&& res, BKT&& ptr, CFG&& cfg)
{
    using idx_t = decltype(cfg.sequence_length);
    if constexpr (show_function)
    {
        std::cerr << __PRETTY_FUNCTION__ << "\n";
    }
    for(idx_t i(1); i<cfg.sequence_length; ++i) 
        if(!is_L[i] && is_L[i-1])
        {   // put LMS-character
            res[--ptr[cfg.map2idx(seq[i])]] = i;
        }
}

}
