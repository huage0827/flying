#include "axon.h"

namespace flyingspore{

    void axon::push(data_pack* _data_pack){

    }

    meta::_meta_axon_type flyingspore::axon::get_type() const{
        return _type;
    }

    p_data_format_t flyingspore::axon::get_data_format() const{
        return _data_format;
    }

    p_actuator_t flyingspore::axon::get_actuator() const{
        return _actuator;
    }

    p_matrix_t operator & (const p_axon_t &_a1, const p_axon_t &_a2){
        return std::make_shared<matrix>();
    }

    p_matrix_t operator | (const p_axon_t &_a1, const p_axon_t &_a2){
        return std::make_shared<matrix>();
    }

    p_chain_t operator >> (const p_axon_t &_a1, const p_axon_t &_a2){
        return std::make_shared<chain>();
    }

    p_chain_t operator << (const p_axon_t &_a1, const p_axon_t &_a2){
        return std::make_shared<chain>();
    }
}



