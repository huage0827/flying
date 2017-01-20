#ifndef flying_spore_axon_builder_h
#define flying_spore_axon_builder_h
#include "axon.h"

namespace flyingspore{

    class axon_builder : public axon{
    public:
        void reset();
        void set_type(_meta_axon_type _t);
        void set_data_format(p_data_format_t _f);
        void set_actuator(const p_actuator_t& _act);
        p_axon_t to_axon() const;
    protected:
        mutable std::mutex _lock_builder;
    };
}



#endif/*flying_spore_axon_builder_h*/
