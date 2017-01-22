#ifndef flying_spore_data_trigger_h
#define flying_spore_data_trigger_h
#include "fsd.h"

namespace flyingspore{

        FS_DEF(data_pack);
        FS_DEF(data_format_path);
        FS_DEF(axon);

        class data_trigger{
    public:
        /*
        1. handel, ���������������trigger����Դ
        2. signal, �������Դ��ʲô�źŻᵼ�´���
        */
        /*for data_pack*/
        data_trigger(const p_data_pack_t& _data_pack, const p_data_format_path_t& _path, data_pack::signal _signal);
        /*for axon*/
        data_trigger(const p_axon_t& _axon, axon::signal _signal);
    };


}



#endif/*flying_spore_data_trigger_h*/