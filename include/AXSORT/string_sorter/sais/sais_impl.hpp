#pragma once
#include <numeric>
#include <vector>
#include <array>
#include <limits>
#include <span>
#include <ranges>
#include <algorithm>
#include "../config.hpp"

namespace AXSORT :: string_sorter :: sais_impl {

const bool show_function = false;

template<typename SEQ, typename RES, typename CFG_>
void sa_is(SEQ&&, RES&&, CFG_&&);


enum Direction { backward, forward };
enum Induce { induceL, induceS };
enum Target { suffix, LMS };

template<typename SEQ, typename ISL, typename CFG>
inline void cal_LS_type(SEQ&&, ISL&&, CFG&&);

template<typename SEQ, typename BKT, typename CFG>
inline void cal_bucket_info(SEQ&&, BKT&&, CFG&&);

template<Direction Dir, typename BKT, typename CFG>
inline void cal_bucket_ptr(BKT&&, BKT&&, CFG&&);

template <typename SEQ, typename RES, typename BKT
    , typename ISL, typename CFG>
inline void put_LMSc(SEQ&&, RES&&, BKT&&, ISL&&, CFG&&);

template<typename SEQ, typename CFG>
inline void sequence_check(SEQ&&, CFG&&);

template<typename SEQ, typename ISL, typename IDX>
inline bool same_LMS_substr(SEQ&&, ISL&&, IDX, IDX);

template<typename RES, typename SEQ, typename ISL, typename idx_t>
inline void cal_new_str(RES&& res, SEQ&& seq, ISL&& is_L
        , idx_t& insert_pos, idx_t& rank);

template<Induce LR, Target Tar, typename RES
    , typename SEQ, typename ISL, typename BKT, typename CFG>
inline void induce_sort(RES&&, SEQ&&, ISL&&, BKT&&, CFG&&);

template<Direction Dir, typename RES, typename IDX, typename CFG_>
inline void collect_element(RES&& res, IDX& len, CFG_&& cfg);

template<typename RES, typename IDX, typename ISL, typename CFG_>
inline void recursive_call_sais(RES&& res, ISL&& is_L
        , IDX new_length, IDX new_alphabet, CFG_&& cfg);

template<typename RES, typename SEQ, typename BKT, typename IDX
    , typename CFG_>
inline void put_LMS_sorted(RES&& res, SEQ&& seq, BKT&& ptr
        , IDX&& len, CFG_&& cfg);

template<typename SEQ, typename RES, typename CFG_>
void sa_is(SEQ&& seq, RES&& res, CFG_&& cfg) 
{

    using CFG = typename std::remove_reference<CFG_>::type;
    using idx_t = typename CFG::res_idx_t;

    idx_t len(cfg.sequence_length);
    idx_t insert_pos(0), rank(0);
    std::vector<bool> is_L(len);        // type = L or not
    std::vector<idx_t> bkt_cnt(cfg.alphabet_size), bkt_ptr(bkt_cnt);

    std::cerr << "sais(len=" << len << ",  alphabet=" << cfg.alphabet_size << ")\n";

    sequence_check(seq, cfg);

    cal_LS_type(seq, is_L, cfg);
    cal_bucket_info(seq, bkt_cnt, cfg);
    std::fill(res.begin(), res.begin()+len, CFG::empty);

    cal_bucket_ptr<forward>(bkt_ptr, bkt_cnt, cfg);
    put_LMSc(seq, res, bkt_ptr, is_L, cfg);

    cal_bucket_ptr<backward>(bkt_ptr, bkt_cnt, cfg);
    induce_sort<induceL, LMS>(res, seq, is_L, bkt_ptr, cfg);

    cal_bucket_ptr<forward>(bkt_ptr, bkt_cnt, cfg);
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
    cal_bucket_ptr<forward>(bkt_ptr, bkt_cnt, cfg);

    put_LMS_sorted(res, seq, bkt_ptr, insert_pos, cfg);
    cal_bucket_ptr<backward>(bkt_ptr, bkt_cnt, cfg);
    induce_sort<induceL, suffix>(res, seq, is_L, bkt_ptr, cfg);

    cal_bucket_ptr<forward>(bkt_ptr, bkt_cnt, cfg);
    induce_sort<induceS, suffix>(res, seq, is_L, bkt_ptr, cfg);
}

template<typename RES, typename SEQ, typename BKT, typename IDX
    , typename CFG_>
inline void put_LMS_sorted(RES&& res, SEQ&& seq, BKT&& ptr
        , IDX&& len, CFG_&& cfg)
{
    using CFG = std::remove_reference<CFG_>::type;
    using idx_t = IDX;

    if constexpr (show_function)
    {
        std::cerr << __PRETTY_FUNCTION__ << "\n";
    }
    for(idx_t i(len); i>0; --i)
    {   // put sorted LMS-suffix into bucket
        res[--ptr[cfg.map2idx(seq[res[i-1]])]] = res[i-1];
        if(ptr[cfg.map2idx(seq[res[i-1]])] != i-1)
            res[i-1] = CFG::empty;
    }
}
template<typename SEQ, typename CFG_>
inline void sequence_check(SEQ&& seq, CFG_&& cfg)
{
    using CFG = std::remove_reference<CFG_>::type;
    if constexpr (show_function)
    {
        std::cerr << __PRETTY_FUNCTION__ << "\n";
    }
    if constexpr (CFG::assertion)
    {
        assert(cfg.sequence_length <= CFG::empty || CFG::empty < 0);
        for(typename CFG::seq_idx_t i(0); i<cfg.sequence_length-1; ++i)
        {
            assert(seq[i] > seq[cfg.sequence_length-1]);
            assert(cfg.map2idx(seq[i]) < cfg.alphabet_size);
        }
   }
}


template<typename SEQ, typename ISL, typename IDX>
inline bool same_LMS_substr(SEQ&& seq, ISL&& is_L, IDX s1, IDX s2)
{
    bool start_L = false;
    IDX i=0;
    while(seq[s1+i] == seq[s2+i] && is_L[s1+i] == is_L[s2+i])
    {
        if(is_L[s1+i])
        {
            start_L = true;
            break;
        }
        ++i;
    }
    if(!start_L) return false;
    while(seq[s1+i] == seq[s2+i] && is_L[s1+i] == is_L[s2+i])
    {
        if(!is_L[s1+i])
        {
            start_L = false;
            break;
        }
        ++i;
    }
    return !start_L;
}


template<typename RES, typename SEQ, typename ISL, typename idx_t>
inline void cal_new_str(RES&& res, SEQ&& seq, ISL&& is_L
        , idx_t& insert_pos, idx_t& rank)
{
    res[insert_pos + (res[0]>>1)] = rank;
    for(idx_t i(1); i<insert_pos; ++i)
    {  // compare if same as prev
        if(!same_LMS_substr(seq, is_L, res[i-1], res[i]))
            ++rank;
        res[insert_pos + (res[i]>>1)] = rank;
    }
    
}

template<typename SEQ, typename ISL, typename CFG>
inline void cal_LS_type(SEQ&& seq, ISL&& IS_L, CFG&& cfg)
{   // cal bucket info (S-type: end of bucket)
    using idx_t = decltype(cfg.sequence_length);

    auto is_L = [&IS_L, &cfg](idx_t i) 
        -> std::remove_reference<ISL>::type::reference {
        if constexpr (cfg.assertion) 
            return IS_L.at(i);
        else
            return IS_L[i];
    };

    if constexpr (show_function)
    {
        std::cerr << __PRETTY_FUNCTION__ << "\n";
    }

    for(idx_t i = cfg.sequence_length-1; i>0; --i)
    {  // cal L/S-type
        if(seq[i-1] == seq[i]) is_L(i-1) = is_L(i);
        // is_L[i-i] = is_L[i];
        else is_L(i-1) = seq[i-1] > seq[i];
    }
}
template <
    typename SEQ, typename RES, typename BKT
  , typename ISL, typename CFG >
inline void put_LMSc(
        SEQ&& seq, RES&& res, BKT&& ptr
      , ISL&& is_L, CFG&& cfg)
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

template<Direction Dir, typename BKT, typename CFG>
inline void cal_bucket_ptr(BKT&& ptr, BKT&& cnt, CFG&& cfg)
{
    using idx_t = decltype(cfg.alphabet_size);
    if constexpr (show_function)
    {
        std::cerr << __PRETTY_FUNCTION__ << "\n";
    }

    if constexpr(Dir == forward)
        ptr[0] = cnt[0];
    else
        ptr[0] = 0;
    for(idx_t i(1); i<cfg.alphabet_size; ++i)
    {
        if constexpr(Dir == forward)
            ptr[i] = ptr[i-1] + cnt[i];
        else
            ptr[i] = ptr[i-1] + cnt[i-1];
    }
}

template<typename SEQ, typename BKT, typename CFG>
inline void cal_bucket_info(SEQ&& seq_, BKT&& cnt_, CFG&& cfg)
{
    using idx_t = decltype(cfg.sequence_length);
    if constexpr (show_function)
    {
        std::cerr << __PRETTY_FUNCTION__ << "\n";
    }

    auto seq = [&seq_, &cfg](idx_t i) 
        -> std::remove_reference<SEQ>::type::reference {
        if constexpr (cfg.assertion) 
            assert(i >= 0 
                    && i < cfg.sequence_length
                    && decltype(seq_.size())(i) < seq_.size());
        return seq_[i];
    };
    auto cnt = [&cnt_, &cfg](decltype(cfg.map2idx(0)) i) 
        -> std::remove_reference<BKT>::type::reference {
        if constexpr (cfg.assertion) 
            assert(i >= 0 
                    && i < cfg.alphabet_size 
                    && decltype(cnt_.size())(i) < cnt_.size());
        return cnt_[i];
    };
    for(idx_t i(0); i<cfg.sequence_length; ++i)
        ++cnt(cfg.map2idx(seq(i)));
}

template<Induce LR, Target Tar, typename RES
    , typename SEQ, typename ISL, typename BKT, typename CFG>
inline void induce_sort(RES&& res
        , SEQ&& seq, ISL&& is_L, BKT&& ptr, CFG&& cfg)
{
    using CFG_ = std::remove_reference<CFG>::type;
    if constexpr (show_function)
    {
        std::cerr << __PRETTY_FUNCTION__ << "\n";
    }

    auto view_res = 
        [&res]() {
            if constexpr (LR == induceL)
                return std::views::all(res);
            else
                return std::views::reverse(res);
        } ();

    auto need_induce = std::views::filter(
            [&is_L](auto i) -> bool
            {
                if(i == CFG_::empty || i == 0) 
                    return false;
                if constexpr (LR == induceL)
                    return is_L[i-1];
                else
                    return !is_L[i-1];
            });

    for (auto& i : view_res | need_induce)
    {
        auto induce_target = i-1;
        if constexpr (Tar == LMS)
        {
            i = CFG_::empty;
            if(induce_target == 0) continue;
        }
        if constexpr (LR == induceL)
            res[ptr[cfg.map2idx(seq[induce_target])]++] = induce_target;
        else
            res[--ptr[cfg.map2idx(seq[induce_target])]] = induce_target;
    }
}
template<Direction Dir, typename RES, typename IDX, typename CFG_>
inline void collect_element(RES&& res, IDX& len, CFG_&& cfg)
{
    using CFG = std::remove_reference<CFG_>::type;
    if constexpr (show_function)
    {
        std::cerr << __PRETTY_FUNCTION__ << "\n";
    }

    auto view_res = 
        [&res]() {
            if constexpr (Dir == forward)
                return std::views::all(res);
            else
                return std::views::reverse(res);
        } ();

    IDX cnt = 0;

    for(auto& i : view_res) if( i != CFG::empty )
    {
        if constexpr (Dir == forward)
        {
            if(&res[cnt] != &i)
            {
                res[cnt] = i;
                i = CFG::empty;
            }
            ++cnt;
        }
        else
        {
            if(&res[cfg.sequence_length-cnt-1] != &i)
            {
                res[cfg.sequence_length-cnt-1] = i;
                i = CFG::empty;
            }
            if(++cnt == len) break;
        }

    }
    len = cnt;
}
template<typename RES, typename IDX, typename ISL, typename CFG_>
inline void recursive_call_sais(RES&& res, ISL&& is_L
        , IDX new_length, IDX new_alphabet, CFG_&& cfg)
{
    using idx_t = IDX;
    using CFG = std::remove_reference<CFG_>::type;
    if constexpr (show_function)
    {
        std::cerr << __PRETTY_FUNCTION__ << "\n";
    }

    auto len = cfg.sequence_length;

    std::span seq_(std::begin(res)+len-new_length, new_length);
    std::span res_(std::begin(res)+len-new_length*2, new_length);

    if constexpr (CFG::assertion) 
    {
        assert(new_length < len);
        assert(seq_.size() == new_length);
        assert(res_.size() == new_length);
    }

    // generate cfg object
    config<decltype(seq_), decltype(res_)> new_cfg;
    new_cfg.alphabet_size = new_alphabet;
    new_cfg.sequence_length = new_length;

    sa_is(seq_, res_, new_cfg);

    for(idx_t i(len-1), j(0); j<new_length; --i)
    {   // receive LMS-substr's positions
        if constexpr (CFG::assertion)
        {   // modify p_str
            if(!is_L.at(i) && is_L.at(i-1))
                res[len-1-j++] = i;
        }
        else
        {
            if(!is_L[i] && is_L[i-1])
                res[len-1-j++] = i;
        }
    }

    for(idx_t i(0); i<new_length; ++i)
    {   // map index to original
        res[i] = seq_[res_[i]];
    }
}

}   // sais_impl

/*
    // induce_sort
    for(idx_t i(0); i<len; ++i) 
        if(res[i] != CFG::empty)
        {   // induce LMS-substr's L-type
            val_t ri = res[i]-1;
            if(ri < 0 || !is_L.at(ri)) continue;
            res[i] = CFG::empty;
            if(ri != 0)
                res[bkt_ptr[cfg.map2idx(seq[ri])]++] = ri;
        }
    for(idx_t i(len); i>0; --i) 
        if(res[i-1] != CFG::empty)
        {   // induce LMS-substr's S-type
            val_t ri = res[i-1]-1;
            if(ri < 0 || is_L.at(ri)) continue;
            res[i-1] = CFG::empty;
            if(ri != 0)
                res[--bkt_ptr[cfg.map2idx(seq[ri])]] = ri;
        }
    for(idx_t i(0); i<len; ++i) 
        if(res[i] != CFG::empty)
        {   // induce L-type by sorted LMS-suffix
            val_t ri = res[i]-1;
            if(ri < 0 || !is_L.at(ri)) continue;
            //if(!is_L.at(res[i])) res[i] = CFG::empty;
            res[bkt_ptr[cfg.map2idx(seq[ri])]++] = ri;
        }

    for(idx_t i(len); i>0; --i) 
        if(res[i-1] != CFG::empty)
        {   // induce S-type
            val_t ri = res[i-1]-1;
            if(ri < 0 || is_L.at(ri)) continue;
            res[--bkt_ptr[cfg.map2idx(seq[ri])]] = ri;
        }
        */
/*
    //using idx_t = typename CFG_::res_idx_t;
    //using val_t = typename CFG_::res_val_t;
    if constexpr (LR == induceL && Tar == LMS)
    {
        using idx_t = typename CFG_::res_idx_t;
        using val_t = typename CFG_::res_val_t;
        for(idx_t i(0); i<cfg.sequence_length; ++i) 
            if(res[i] != CFG_::empty)
            {
                val_t ri = res[i]-1;
                if(ri < 0 || !is_L.at(ri)) continue;
                res[i] = CFG_::empty;
                if(ri != 0)
                    res[ptr[cfg.map2idx(seq[ri])]++] = ri;
            }
        return;
    }
    */
    /*
    for(idx_t i(1); i<len; ++i) if(res[i] != CFG::empty) // is LMS
    {   // collect all LMS-substr
        res[insert_pos] = res[i];
        if(insert_pos++ != i)
            res[i] = CFG::empty;
    }
    */
    /*
    for(idx_t i(len-1), j(0); j<insert_pos; --i)
        if(res[i] != CFG::empty)
        {   // collect all new-string's character
            res[len-1-j] = res[i];
            if(len-1-j++ != i)
                res[i] = CFG::empty;
        }
        */
