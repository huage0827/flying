#ifndef flying_spore_fsd_h
#define flying_spore_fsd_h
#include "core.h"
#include <memory>


#define  FS_DEF(classname)  \
    class classname;                \
    typedef std::shared_ptr<classname> p_##classname##_t;


namespace flyingspore{

}



#endif/*flying_spore_fsd_h*/