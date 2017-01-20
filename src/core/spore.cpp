#include "spore.h"

namespace flyingspore{

    p_matrix_t operator & (const p_spore_t &_a1, const p_spore_t &_a2){
        return std::make_shared<matrix>();
    }

    p_matrix_t operator | (const p_spore_t &_a1, const p_spore_t &_a2){
        return std::make_shared<matrix>();
    }

    p_matrix_t operator >> (const p_spore_t &_a1, const p_spore_t &_a2){
        return std::make_shared<matrix>();
    }

    p_matrix_t operator << (const p_spore_t &_a1, const p_spore_t &_a2){
        return std::make_shared<matrix>();
    }

    spore::spore(p_spore_t _parent /*= nullptr*/): fobject(_parent)
    {

    }

    p_spore_t spore::clone()
    {
        return p_spore_t();
    }

    p_spore_t spore::newborn()
    {
        return p_spore_t();
    }

    void spore::walk_axon(const walk_axon_function &_func)
    {
        if(!_func) return;
        for (auto ite = _axon_array.begin(); ite != _axon_array.end(); ite ++){
            if(!(_func)(ite->first, ite->second))
                return;
        }
    }

}

