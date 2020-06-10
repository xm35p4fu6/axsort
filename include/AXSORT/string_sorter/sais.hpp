#pragma once
#include <numeric>
#include <vector>
#include <array>
#include <limits>
#include <span>

namespace AXSORT { namespace string_sorter {

class sais
{
  public:

    // idx_type: use for implementation's for loop counter, can be unsigned
    // val_type: can't be unsigned
    // assertion: open all assert() if true. useful for debug.
    // map2idx(): map the original sequence's character to bucket's index
    // map2val(): reverse of map2idx()
    template<typename SEQ_, typename RES_>
    class default_config 
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

    template<typename SEQ, typename RES, typename CFG = default_config<SEQ, RES>>
    void sort(SEQ&& seq, RES&& res, CFG cfg = CFG())
    {
        if(cfg.sequence_length == 0) 
            cfg.sequence_length = seq.size();
        sa_is(seq, res, cfg);
    }

  private:
    template<typename SEQ, typename INDEX_T>
    void sequence_check(SEQ&& seq, INDEX_T N)
    {
        for(INDEX_T i(0); i+1<N; ++i) 
            assert(seq[i] > seq[N-1]);
    }

    template<typename SEQ, typename ISL, typename IDX>
    bool same_LMS_substr(SEQ&&, ISL&&, IDX, IDX);

    template<typename SEQ, typename RES, typename CFG_>
    void sa_is(SEQ&&, RES&&, CFG_&& cfg = CFG_());

    template<typename SEQ, typename ISL, typename CFG>
    void cal_LS_type(SEQ&&, ISL&&, CFG&&);
};

template<typename SEQ, typename ISL, typename IDX>
bool sais::same_LMS_substr(SEQ&& seq, ISL&& is_L, IDX s1, IDX s2)
{
    bool start_L = false;
    IDX i=0;
    while(seq[s1+i] == seq[s2+i] && is_L.at(s1+i) == is_L.at(s2+i))
    {
        if(is_L.at(s1+i))
        {
            start_L = true;
            break;
        }
        ++i;
    }
    if(!start_L) return false;
    while(seq[s1+i] == seq[s2+i] && is_L.at(s1+i) == is_L.at(s2+i))
    {
        if(!is_L.at(s1+i))
        {
            start_L = false;
            break;
        }
        ++i;
    }
    return !start_L;
}

template<typename SEQ, typename ISL, typename CFG>
inline void sais::cal_LS_type(SEQ&& seq, ISL&& IS_L, CFG&& cfg)
{
    using idx_t = decltype(cfg.sequence_length);

    auto is_L = [&IS_L, &cfg](idx_t i) 
        -> std::remove_reference<ISL>::type::reference {
        //-> std::remove_reference<ISL>::type::value_type& {
        // control assertion on/off
        if constexpr (cfg.assertion) 
            return IS_L.at(i);
        else
            return IS_L[i];
    };

    for(idx_t i = cfg.sequence_length-1; i>0; --i)
    {  // cal L/S-type
        if(seq[i-1] == seq[i]) is_L(i-1) = is_L(i);
        // is_L[i-i] = is_L[i];
        else is_L(i-1) = seq[i-1] > seq[i];
    }
}

template<
    typename SEQ
  , typename RES
  , typename CFG_ = sais::default_config<SEQ,RES>
    >
void sais::sa_is(
      SEQ&& seq
    , RES&& res
    , CFG_&& cfg
    ) 
{

    using CFG = typename std::remove_reference<CFG_>::type;
    using idx_t = typename CFG::res_idx_t;
    using val_t = typename CFG::res_val_t;

    idx_t len(cfg.sequence_length);
    std::vector<bool> is_L(len);        // type = L or not
    std::vector<idx_t> bkt_cnt(cfg.alphabet_size), bkt_ptr(bkt_cnt);

    if constexpr (CFG::assertion) 
        sequence_check(seq, len);

    cal_LS_type(seq, is_L, cfg);

    // cal bucket info (S-type: end of bucket)
    for(idx_t i(0); i<len; ++i) ++bkt_cnt[cfg.map2idx(seq[i])];
    bkt_ptr[0] = bkt_cnt[0];
    for(idx_t i(1); i<bkt_cnt.size(); ++i) 
        bkt_ptr[i] = bkt_ptr[i-1] + bkt_cnt[i];

    // std::fill(res.begin(), res.end(), CFG::empty);      // clear
    for(idx_t i(0); i<len; ++i) res[i] = CFG::empty;

    for(idx_t i(1); i<len; ++i) if(!is_L.at(i) && is_L.at(i-1))
    {   // put LMS-character
        res[--bkt_ptr[cfg.map2idx(seq[i])]] = i;
    }

    // bkt_ptr[0] = 0; // it is already = 0 
    for(idx_t i(1); i<bkt_cnt.size(); ++i) 
        bkt_ptr[i] = bkt_ptr[i-1] + bkt_cnt[i-1];

    for(idx_t i(0); i<len; ++i) 
        if(res[i] != CFG::empty)
        {   // induce LMS-substr's L-type
            val_t ri = res[i]-1;
            if(ri < 0 || !is_L.at(ri)) continue;
            res[i] = CFG::empty;
            if(ri != 0)
                res[bkt_ptr[cfg.map2idx(seq[ri])]++] = ri;
        }

    bkt_ptr[0] = bkt_cnt[0];
    for(idx_t i(1); i<bkt_cnt.size(); ++i) 
        bkt_ptr[i] = bkt_ptr[i-1] + bkt_cnt[i];

    for(idx_t i(len); i>0; --i) 
        if(res[i-1] != CFG::empty)
        {   // induce LMS-substr's S-type
            val_t ri = res[i-1]-1;
            if(ri < 0 || is_L.at(ri)) continue;
            res[i-1] = CFG::empty;
            if(ri != 0)
                res[--bkt_ptr[cfg.map2idx(seq[ri])]] = ri;
        }

    res[0] = len-1;     // sentinel

    idx_t insert_pos(1), rank(0);
    bool have_same_rank = false;
    for(idx_t i(1); i<len; ++i) if(res[i] != CFG::empty && res[i] != 0) // is LMS
    {   // colllect all LMS-substr
        res[insert_pos] = res[i];
        if(insert_pos++ != i)
            res[i] = CFG::empty;
    }

    res[insert_pos + (res[0]>>1)] = rank;
    for(idx_t i(1); i<insert_pos; ++i)
    {  // compare if same as prev
        if(same_LMS_substr(seq, is_L, res[i-1], res[i]))
            have_same_rank = true;
        else
            ++rank;
        res[insert_pos + (res[i]>>1)] = rank;
    }

    for(idx_t i(len-1), j(0); j<insert_pos; --i)
        if(res[i] != CFG::empty)
        {   // collect all new-string's character
            res[len-1-j++] = res[i];
            res[i] = CFG::empty;
        }

    if(have_same_rank)
    {   
        // modify cfg object
        CFG new_cfg;
        new_cfg.alphabet_size = rank+1;
        new_cfg.sequence_length = insert_pos;
        // if we decide that only accept type which suppose iterator
         std::span seq_(std::begin(res)+len-insert_pos, insert_pos);
         std::span res_(std::begin(res)+len-insert_pos*2, insert_pos);
        //val_t* seq_(&res[len-insert_pos]);
        //val_t* res_(&res[len-insert_pos*2]);

        sa_is(seq_, res_, new_cfg);

        for(idx_t i(len-1), j(0); j<insert_pos; --i)
        {   // receive LMS-substr's positions
            if(!is_L.at(i) && is_L.at(i-1))
            {   // modify p_str
                res[len-1-j++] = i;
            }
        }

        for(idx_t i(0); i<insert_pos; ++i)
        {   // map index to original
            res[i] = seq_[res_[i]];
        }
    }

    // clear bucket
    for(idx_t i(insert_pos); i<len; ++i)
        res[i] = CFG::empty;

    bkt_ptr[0] = bkt_cnt[0];
    for(idx_t i(1); i<bkt_cnt.size(); ++i) 
        bkt_ptr[i] = bkt_ptr[i-1] + bkt_cnt[i];

    for(idx_t i(insert_pos); i>0; --i)
    {   // put sorted LMS-suffix into bucket
        res[--bkt_ptr[cfg.map2idx(seq[res[i-1]])]] = res[i-1];
        if(bkt_ptr[cfg.map2idx(seq[res[i-1]])] != i-1)
            res[i-1] = CFG::empty;
    }

    // bkt_ptr[0] = 0; // it is already = 0 
    for(idx_t i(1); i<bkt_cnt.size(); ++i) 
        bkt_ptr[i] = bkt_ptr[i-1] + bkt_cnt[i-1];

    for(idx_t i(0); i<len; ++i) 
        if(res[i] != CFG::empty)
        {   // induce L-type by sorted LMS-suffix
            val_t ri = res[i]-1;
            if(ri < 0 || !is_L.at(ri)) continue;
            if(!is_L.at(res[i])) res[i] = CFG::empty;
            res[bkt_ptr[cfg.map2idx(seq[ri])]++] = ri;
        }
    res[0] = len-1;     // sentinel

    bkt_ptr[0] = bkt_cnt[0]; 
    for(idx_t i(1); i<bkt_cnt.size(); ++i) 
        bkt_ptr[i] = bkt_ptr[i-1] + bkt_cnt[i];

    for(idx_t i(len); i>0; --i) 
        if(res[i-1] != CFG::empty)
        {   // induce S-type
            val_t ri = res[i-1]-1;
            if(ri < 0 || is_L.at(ri)) continue;
            res[--bkt_ptr[cfg.map2idx(seq[ri])]] = ri;
        }
}

}}
