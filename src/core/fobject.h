#ifndef FlyingSpore_fobject_h
#define FlyingSpore_fobject_h

#include <memory>
#include <list>
#include <mutex>

namespace fs{
    class fobject;
    class fcontainer;
    typedef std::shared_ptr<fobject> p_object_t;
    typedef std::list<fobject> object_list_t;
    typedef std::shared_ptr<fcontainer> p_container_t;

    class fobject
    {
    public:
        fobject():_parent_object(nullptr){
        }
        fobject(const p_object_t& _parent):_parent_object(_parent){
        }
        virtual ~fobject(){
        }
        p_object_t get_parent(){
            return _parent_object;
        }
    protected:
        p_object_t _parent_object;
    };


    class fcontainer: public fobject{
    public:
        fcontainer(p_object_t _parent = nullptr):_parent_object(_parent){
        }
        p_object_t add_child(p_object_t _p){
            if(!_p){
                std::lock_guard<std::mutex> lk(_lock);
                _childs.push_back(_p);
                _p->_parent_object = this;
            }
            return _p;
        }

        fcontainer(const fcontainer& _other):fobject(_other._parent_object){
            //std::unique_lock<std::mutex> lock1(_lock, std::defer_lock);
            //std::unique_lock<std::mutex> lock2(_other._lock, std::defer_lock);
            //std::lock(lock1, lock2);
            //_childs = _other._childs;
            std::lock_guard lock(_other._lock);
            _childs = _other._childs;
        }

        fcontainer(fcontainer&& _other){
            _parent_object = _other._parent_object;
            std::lock_guard<std::mutex> lk(_lock);
            std::swap(_childs, _other._childs);
        }

        fcontainer(const object_list_t& _object_list):_childs(_object_list){
        }

        fcontainer(object_list_t&& _object_list){
            std::swap(_childs, _object_list);
        }

        p_object_t remove_child(p_object_t _p){
            if(!_p){
                std::lock_guard<std::mutex> lk(_lock);
                _childs.remove(_p);
                _parent_object = nullptr;
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
            return _childs.size() > 0
        }
    protected:
        mutable std::mutex _lock;
        object_list_t _childs;
    };
}



#endif/*FlyingSpore_fobject_h*/