#include "spore_builder.h"

namespace flyingspore{

    p_data_format_t spore_builder::add_data_format(std::string _format_name, const p_data_format_t& _p_data_format_t)
    {
        if(!_p_data_format_t)
            return nullptr;
        std::lock_guard<std::mutex> lk(_lock_builder);
        _context_array[_format_name] = _p_data_format_t;
        return _p_data_format_t;
    }

    p_axon_t spore_builder::add_axon(const std::string &_axon_name, const axon_builder& _axon_builder)
    {
        p_axon_t _p = _axon_builder.to_axon();
        return add_axon(_axon_name, _p);
    }

    p_axon_t spore_builder::add_axon(std::string _axon_name, const p_axon_t& _axon)
    {
        std::lock_guard<std::mutex> lk(_lock_builder);
        //_action_list.merge(_axon_builder._action_list);
        _axon_array[_axon_name] = _axon;
        return _axon;
    }

    void spore_builder::add_trigger(const data_trigger& _trigger, const data_handler& _action)
    {
        std::lock_guard<std::mutex> lk(_lock_builder);
        _action_list.push_back(std::make_shared<data_action>(_trigger, _action));
    }

}

