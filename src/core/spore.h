#ifndef flying_spore_spore_h
#define flying_spore_spore_h
#include <memory>
#include <map>
#include <functional>
#include <list>
#include "axon.h"
#include "data_action.h"

namespace flyingspore{

    class spore_builder;
    class spore;
    typedef std::shared_ptr<spore> p_spore_t;
    typedef std::function< bool(const std::string&, const p_axon_t& )> walk_axon_function;

    p_matrix_t operator & (const p_spore_t &_a1, const p_spore_t &_a2);
    p_matrix_t operator | (const p_spore_t &_a1, const p_spore_t &_a2);
    p_matrix_t operator >> (const p_spore_t &_a1, const p_spore_t &_a2);
    p_matrix_t operator << (const p_spore_t &_a1, const p_spore_t &_a2);

    class spore : public fobject{
    public:
        spore(p_spore_t _parent = nullptr);
        p_spore_t clone();

        p_spore_t newborn();

        void walk_axon(const walk_axon_function &_func);
    protected:
        std::map<std::string, p_data_format_t> _context_array;
        std::map<std::string, p_axon_t> _axon_array;
        std::list<p_data_action_t> _action_list;
    };
}



#endif/*flying_spore_spore_h*/