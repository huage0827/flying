#include "fobject.h"

namespace flyingspore{

    fobject::fobject(const p_object_t& _parent /*= nullptr*/) :_parent_object(_parent)
    {

    }

    flyingspore::p_object_t fobject::get_parent()
    {
        return _parent_object;
    }

}

