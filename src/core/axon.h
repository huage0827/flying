#ifndef flying_spore_axon_h
#define flying_spore_axon_h
#include "fsd.h"
#include "fobject.h"

namespace flyingspore{
    using namespace meta;

    FS_DEF(axon);
    FS_DEF(matrix);
    FS_DEF(chain);
    FS_DEF(data_pack_builder);
    FS_DEF(data_format);
    FS_DEF(actuator);
    FS_DEF(data_pack);

    p_matrix_t operator & (const p_axon_t &_a1, const p_axon_t &_a2);
    p_matrix_t operator | (const p_axon_t &_a1, const p_axon_t &_a2);
    p_chain_t operator >> (const p_axon_t &_a1, const p_axon_t &_a2);
    p_chain_t operator << (const p_axon_t &_a1, const p_axon_t &_a2);


    class axon :fobject{
    public:
        enum  signal{data_in};
        void push(data_pack* _data_pack);
        _meta_axon_type get_type() const;
        p_data_format_t get_data_format() const;
        p_actuator_t get_actuator() const;

    protected:
        friend class cluster;
        friend class spore;
        friend class axon_builder;

        _meta_axon_type _type{UNKNOWN};
        p_data_format_t _data_format{nullptr};
        p_actuator_t _actuator{nullptr};
    };
}



#endif/*flying_spore_axon_h*/
