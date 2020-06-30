#pragma once
#include "sais/sais_impl.hpp"
#include "sais/psacak_impl.hpp"
#include "sais/sacak_impl.hpp"
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
        switch(cfg.method)
        {
            case Method::SAIS:
                sais_impl::sa_is(seq, res, cfg);
                break;
            case Method::SACAK:
                sacak_impl::saca_k(seq, res, cfg);
                break;
            case Method::PSACAK:
                psacak_impl::psaca_k(seq, res, cfg);
                break;
        }
    }

};

}

