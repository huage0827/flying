#ifndef flying_spore_spore_builder_h
#define flying_spore_spore_builder_h
#include <mutex>
#include "axon.h"
#include "axon_builder.h"
#include "data_trigger.h"
#include "data_action.h"
#include "spore.h"
#include "data_format.h"

namespace flyingspore{
    class spore_builder: public spore{
    public:
        p_data_format_t add_data_format(std::string _format_name,  const p_data_format_t& _p_data_format_t);
        p_axon_t add_axon(const std::string &_axon_name,  const axon_builder& _axon_builder);
        p_axon_t add_axon(std::string _axon_name,  const p_axon_t& _axon);
        void add_trigger(const data_trigger& _trigger, const data_handler& _action);
    protected:
        std::mutex _lock_builder;
    };
}



#endif/*flying_spore_spore_builder_h*/