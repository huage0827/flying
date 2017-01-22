#ifndef flying_spore_chain_h
#define flying_spore_chain_h
#include "fsd.h"
#include "fobject.h"

namespace flyingspore{

    FS_DEF(axon);

    /*
    链，两端是axon，一个in， 一个out
    */
    class chain : fobject{
    public:
        chain(p_object_t _parent = nullptr);

        chain(p_axon_t _a1, p_axon_t _a2);
        bool is_valid();

    protected:
        p_axon_t _axon_in{nullptr};
        p_axon_t _axon_out{nullptr};
    };

}



#endif/*flying_spore_chain_h*/