#ifndef flying_spore_fcontainer_h
#define flying_spore_fcontainer_h

#include "fobject.h"
#include <list>
#include <mutex>

namespace flyingspore{


    template<typename T>
    class fcontainer: public fobject{
    public:
        typedef std::shared_ptr<T> p_T_t;
        typedef std::list<p_T_t> T_list_t;
        typedef std::function< bool(const p_T_t& )> walk_object_function;

        fcontainer(p_object_t _parent = nullptr):fobject(_parent){
        }
        p_object_t add_child(p_T_t _p){
            if(!_p){
                std::lock_guard<std::mutex> lk(_lock);
                _childs.push_back(_p);
                _p->_parent_object = p_object_t(this);
            }
            return _p;
        }

        fcontainer(const fcontainer& _other):fobject(_other._parent_object){
            std::lock_guard<std::mutex> lock(_other._lock);
            _childs = _other._childs;
        }

        fcontainer(fcontainer&& _other){
            _parent_object = _other._parent_object;
            std::lock_guard<std::mutex> lk(_lock);
            std::swap(_childs, _other._childs);
        }

        fcontainer(const T_list_t& _object_list):_childs(_object_list){
        }

        fcontainer(T_list_t&& _object_list){
            std::swap(_childs, _object_list);
        }

        p_object_t remove_child(p_T_t _p){
            if(!_p){
                std::lock_guard<std::mutex> lk(_lock);
                _childs.remove(_p);
                _p->_parent_object = nullptr;
            }
            return _p;
        }

        object_list_t::size_type get_child_count() const{
            std::lock_guard<std::mutex> lk(_lock);
            return _childs.size();
        }

        void clear_childs(){
            std::lock_guard<std::mutex> lk(_lock);
            _childs.clear();
        }

        void walk_childs(const walk_object_function &_func){
            if(!_func) return;
            std::lock_guard<std::mutex> lk(_lock);
            for (const auto &ite : _childs){
                if(!(_func)(ite))
                    return;
            }
        }

        bool has_childs(){
            std::lock_guard<std::mutex> lk(_lock);
            return _childs.size() > 0;
        }
    protected:
        mutable std::mutex _lock;
        T_list_t _childs;
    };
}



#endif/*flying_spore_fcontainer_h*/