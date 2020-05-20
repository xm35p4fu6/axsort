#pragma once
//#include <AXSORT/string_sorter/sais/*.hpp>
#include <numeric>
#include <vector>
#include <array>
#include <limits>

namespace AXSORT { namespace string_sorter {

class sais
{
    template<typename SEQ_, typename RES_>
    class default_config 
    {
        using SEQ = typename std::remove_reference<SEQ_>::type;
        using RES = typename std::remove_reference<RES_>::type;
      public:
        static constexpr bool sequence_need_check = true;
        static constexpr int alphabet_size = 
            std::numeric_limits<typename SEQ::value_type>::max();
        static constexpr auto empty =
            std::numeric_limits<typename RES::value_type>::max();

        using idx_type = typename SEQ::size_type;

        static inline idx_type map2idx(typename SEQ::value_type v) { return v; }
        static inline typename SEQ::value_type map2val(idx_type v) { return v; }
    };

    template<typename SEQ>
    void sequence_check(SEQ&& seq, size_t s, size_t N)
    {
        for(size_t i(0); i+1<N; ++i)
            assert(seq[s+i] > seq[s+N-1]);
    }

    /* 
     * 把 induce L, R 寫進 sais/induce_sort.hpp
     *
     * */
    template<
        typename SEQ
        , typename RES
        , typename CFG = default_config<SEQ,RES>
        , bool level0 = true
        >   // ss : string offset
    void sa_is(SEQ&& seq, RES&& res, CFG cfg = CFG()
            , typename CFG::idx_type ss = 0
            , typename CFG::idx_type len = 0)
    {
        if constexpr (level0)
            len = seq.size();

        //std::cout << SEQ::haha();
        using idx_t = typename CFG::idx_type;
        std::vector<bool> is_L(len);        // type = L or not
        std::vector<idx_t> bkt_cnt(CFG::alphabet_size), bkt_ptr(bkt_cnt);

        if constexpr (CFG::sequence_need_check) 
            sequence_check(seq, ss, len);

        for(idx_t i = len-2; i>=0; --i)
        {  // cal L/S-type
            if(seq[ss+i] == seq[ss+i+1]) is_L[i] = is_L[i+1];
            else is_L[i] = seq[ss+i] > seq[ss+i+1];
        }

        // cal bucket info (S-type: end of bucket)
        for(idx_t i(0); i<seq.size(); ++i) ++bkt_cnt[CFG::map2idx(seq[i])];
        bkt_ptr[0] = bkt_cnt[0];
        for(idx_t i(1); i<bkt_cnt.size(); ++i) 
            bkt_ptr[i] = bkt_ptr[i-1] + bkt_cnt[i];

        std::fill(res.begin(), res.end(), CFG::empty);      // clear

        for(idx_t i(1); i<len; ++i) if(!is_L[i] && is_L[i-1])
        {   // put LMS-character
            res[--bkt_ptr[CFG::map2idx(seq[i])]] = i;
        }

        // bkt_ptr[0] = 0; // it is already = 0 
        for(idx_t i(1); i<bkt_cnt.size(); ++i) 
            bkt_ptr[i] = bkt_ptr[i-1] + bkt_cnt[i-1];

        for(idx_t i(0); i<len; ++i) 
        {   // induce LMS-substr's L-type
            idx_t ri = res[i]-1;
            res[i] = CFG::empty;
            if(ri < 0 || !is_L[ri]) continue;
            res[bkt_ptr[CFG::map2idx(seq[ri])]++] = ri;
        }

        // bkt_ptr[0] = bkt_cnt[0];  // it is alread = 1
        for(idx_t i(1); i<bkt_cnt.size(); ++i) 
            bkt_ptr[i] = bkt_ptr[i-1] + bkt_cnt[i];

        for(idx_t i(len-1); i>=0; --i) 
        {   // induce LMS-substr's S-type
            idx_t ri = res[i]-1;
            res[i] = CFG::empty;
            if(ri < 0 || is_L[ri]) continue;
            res[--bkt_ptr[CFG::map2idx(seq[ri])]] = ri;
        }
    }

    template<typename SEQ, typename RES, typename CFG = default_config<SEQ, RES>>
    void sample(SEQ&& seq, RES&& res, CFG cfg = CFG())
    {
        std::iota(res.begin(), res.end(), 0);
        std::sort(res.begin(), res.end(), [&](int a, int b)
                {
                return seq.substr(a) < seq.substr(b); 
                });
    }

  public:
    template<typename SEQ, typename RES, typename CFG = default_config<SEQ, RES>>
    void sort(SEQ&& seq, RES&& res, CFG cfg = CFG())
    {
        //sa_is(seq, res, cfg);
        sample(seq, res, cfg);
    }
};

}}
