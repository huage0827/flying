#ifndef flying_spore_neure_h
#define flying_spore_neure_h
#include "fcontainer.h"

namespace flyingspore{

    class neure;
    typedef std::shared_ptr<neure> p_neure_t;

        /*
    neure 一组以确定方式聚合的axon,chain或者子neure
    */
    enum neure_type{
        _unknow = 0,
        _addition = 1 << 16, 
        _multiplication = 1 << 17
    };

    typedef std::list<p_object_t> object_list_t; 

    class neure : public fcontainer<fobject>{
    public:

        neure(p_object_t _parent = nullptr);
        neure(const neure& _other);
        neure(neure&& _other);
        neure(neure_type _t, object_list_t &&_chs);
        neure(neure_type _t, const object_list_t &_chs);
        neure_type get_type() const;

    protected:
        neure_type _type{_unknow};
    };
}



#endif/*flying_spore_neure_h*/