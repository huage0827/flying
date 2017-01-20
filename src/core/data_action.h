#ifndef flying_spore_data_action_h
#define flying_spore_data_action_h
#include <functional>
#include "data_trigger.h"
#include "data_context.h"
#include "axon.h"

namespace flyingspore{

    typedef std::function< void(const data_context &, const axon &, const data_pack & )> data_handler;

    class data_action;
    typedef std::shared_ptr<data_action> p_data_action_t;

    class data_action{
    public:
        data_action(const data_trigger _t, const data_handler &_h):_trigger(_t), _handler(_h){
        }
        data_trigger _trigger;
        data_handler _handler;
    };
}



#endif/*flying_spore_data_action_h*/