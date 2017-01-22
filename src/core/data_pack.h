#ifndef flying_spore_data_pack_h
#define flying_spore_data_pack_h
#include <memory>
#include "data_value.h"

namespace flyingspore{

    class data_pack{
    public:
        enum signal{before_changed,  after_changed};

        const data_value& operator[](std::string _path);

        const data_value& operator[](char* _path);

    protected:

    };
}



#endif/*flying_spore_data_pack_h*/