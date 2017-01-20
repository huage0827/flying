#ifndef flying_spore_data_context_h
#define flying_spore_data_context_h
#include "data_pack.h"
#include "spore.h"

namespace flyingspore{
    class data_context : public data_pack{
    public:
    protected:
        p_spore_t _owner;
    };
}



#endif/*flying_spore_data_context_h*/