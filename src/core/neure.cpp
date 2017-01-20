#include "neure.h"

namespace flyingspore{

    neure::neure(p_object_t _parent /*= nullptr*/) :fcontainer(_parent)
    {

    }

    neure::neure(const neure& _other)
    {
        _parent_object = _other._parent_object;
        _type = _other._type;
        std::lock_guard<std::mutex> lock(_other._lock);
        _childs = _other._childs;
    }

    neure::neure(neure&& _other)
    {
        _parent_object = _other._parent_object;
        _type = _other._type;
        std::lock_guard<std::mutex> lk(_lock);
        std::swap(_childs, _other._childs);
    }

    neure::neure(neure_type _t, object_list_t &&_chs) :_type(_t)
    {
        std::swap(_childs, _chs);
    }

    neure::neure(neure_type _t, const object_list_t &_chs) :_type(_t)
    {
        _childs = _chs;
    }

    flyingspore::neure_type neure::get_type() const
    {
        return _type;
    }

}

