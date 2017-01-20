#include "axon_builder.h"

namespace flyingspore{

    void axon_builder::reset(){
        std::lock_guard<std::mutex> lk(_lock_builder);
        _type = _meta_axon_type::UNKNOWN;
        _data_format = nullptr;
        _actuator = nullptr;
    }

    void axon_builder::set_type(_meta_axon_type _t){
        std::lock_guard<std::mutex> lk(_lock_builder);
        _type = _t;
    }

    void axon_builder::set_data_format(p_data_format_t _f){
        std::lock_guard<std::mutex> lk(_lock_builder);
        _data_format = _f;
    }

    void axon_builder::set_actuator(const p_actuator_t& _act){
        std::lock_guard<std::mutex> lk(_lock_builder);
        _actuator = _act;
    }

    flyingspore::p_axon_t axon_builder::to_axon() const{
        std::lock_guard<std::mutex> lk(_lock_builder);
        p_axon_t _a = std::make_shared<axon>();
        _a->_type = _type;
        _a->_data_format = _data_format;
        _a->_actuator = _actuator;
        return _a;
    }

}

