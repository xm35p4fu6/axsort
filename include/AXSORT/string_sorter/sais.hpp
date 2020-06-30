#pragma once
#include "sais/sais_impl.hpp"
#include "config.hpp"

namespace AXSORT :: string_sorter {

struct sais
{
    template<typename SEQ, typename RES, typename CFG = config<SEQ, RES>>
    void sort(SEQ&& seq, RES&& res, CFG cfg = CFG())
    {
        if(cfg.sequence_length == 0) 
        {
            cfg.sequence_length = seq.size();
        }
        sais_impl::sa_is(seq, res, cfg);
    }

};

}
