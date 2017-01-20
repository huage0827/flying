#ifndef flying_spore_data_trigger_h
#define flying_spore_data_trigger_h
#include "data_pack.h"
#include "data_format.h"
#include "axon_builder.h"

namespace flyingspore{
        class data_trigger{
    public:
        /*
        1. handel, ���������������trigger����Դ
        2. signal, �������Դ��ʲô�źŻᵼ�´���
        */
        /*for data_pack*/
        data_trigger(const p_data_pack_t& _data_pack, const data_format_path& _path, data_pack::signal _signal);
        /*for axon*/
        data_trigger(const p_axon_t& _axon, axon::signal _signal);
    };


}



#endif/*flying_spore_data_trigger_h*/