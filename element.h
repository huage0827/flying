#ifndef ELEMENT_H_INCLUDED
#define ELEMENT_H_INCLUDED
#include "core.h"
#include "pack_pool.h"

namespace sf
{

    class data_pack
    {
    public:
        enum signal{before_changed,  after_changed};
    };

    class nenre : public _meta_neure{
    public:
        nenre(){
            //std::call_once()
        }
    protected:
        static std::unique_ptr<pack_pool<data_pack>> _g_pool;
    };

    class axon{

        /* */
        enum  signal{data_in};
        data_pack_builder& get_data_pack_builder();
        void push(data_pack&& _data_pack);
    };

}





#endif // ELEMENT_H_INCLUDED