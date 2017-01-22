#ifndef flying_spore_data_action_h
#define flying_spore_data_action_h
#include <functional>
#include "fsd.h"
#include "data_trigger.h"

namespace flyingspore{

    FS_DEF(data_context);
    FS_DEF(axon);
    FS_DEF(data_pack);

    typedef std::function< void(p_data_context_t,p_axon_t, p_data_pack_t)> data_handler;

    class data_action{
    public:
        data_action(const data_trigger _t, const data_handler &_h):_trigger(_t), _handler(_h){
        }
        data_trigger _trigger;
        data_handler _handler;
    };
}



#endif/*flying_spore_data_action_h*/