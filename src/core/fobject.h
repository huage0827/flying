#ifndef flying_spore_fobject_h
#define flying_spore_fobject_h
#include <memory>

namespace flyingspore{

    class fobject;
    typedef std::shared_ptr<fobject> p_object_t;
    template<typename T> class fcontainer;

    class fobject
    {
    public:
        fobject(const p_object_t& _parent = nullptr);
        virtual ~fobject(){
        }
        p_object_t get_parent();
    protected:
        friend fcontainer<fobject>;
        p_object_t _parent_object{nullptr};
    };
}



#endif/*flying_spore_fobject_h*/