#include "chain.h"

namespace flyingspore{

    chain::chain(p_object_t _parent /*= nullptr*/) :fobject(_parent)
    {

    }

    chain::chain(p_axon_t _a1, p_axon_t _a2)
    {
        if(_a1->get_type() == ~_a2->get_type()){
            //ÀàÐÍÆ¥ÅäÅÐ¶Ï
            //...
            //...
            if(_a1->get_type() == _meta_axon_type::IN_AXON)
                _axon_in = _a1;
            else
                _axon_out = _a1;
            if(_a2->get_type() == _meta_axon_type::IN_AXON)
                _axon_in = _a2;
            else
                _axon_out = _a2;
        }
    }

    bool chain::is_valid()
    {
        return _axon_in && _axon_out;
    }

}

