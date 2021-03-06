/*!
* \file element.h
* \date 2016/12/06 18:05
*
* \author kicsy
* Contact: lmj07luck@126.com
*
* \brief 
*
* 本文档是"flying spore" 原型设计草案的一部分，它在框架层面描述应该提供哪些API给使用者。 
*
* \note
*/

#ifndef ELEMENT_H_INCLUDED
#define ELEMENT_H_INCLUDED
#include <mutex>
#include "core.h"
#include "pack_pool.h"

namespace sf
{

    class virtual_t;
    class base_object;
    class object_container;

    class cluster;
    class data_trigger;
    class data_format;

    //数值定位符号，类似于: fileInfo/state这样的路径
    class data_format_path;

    class data_pack;
    class data_context;
    class data_pack_builder;

    class chain;
    class axon;
    class spore;
    class spore_odd;
    class actuator;
    class neure;
    class data_action;

    class polymer;


    typedef std::shared_ptr<virtual_t> p_virtual_t;
    typedef std::shared_ptr<base_object> p_object_t;
    typedef std::list<p_object_t> object_list_t;
    typedef std::shared_ptr<object_container> p_object_container_t;

    
    typedef std::shared_ptr<data_format> p_data_format_t;

    typedef std::unique_ptr<data_pack> p_data_pack_t;
    typedef std::unique_ptr<data_pack_builder> p_data_pack_builder_t;

    typedef std::unique_ptr<chain> p_chain_t;
    typedef std::list<p_chain_t> chain_list_t;
    typedef std::shared_ptr<chain_list_t> p_chain_list_t;


    typedef std::shared_ptr<axon> p_axon_t;
    typedef std::list<p_axon_t> axon_list_t;

    typedef std::shared_ptr<spore> p_spore_t;
    typedef std::list<p_spore_t> spore_list_t;
    typedef std::shared_ptr<spore_list_t> p_spore_list_t;

    typedef std::shared_ptr<data_context> p_data_context_t;

    typedef std::shared_ptr<actuator> p_actuator_t;

    typedef std::shared_ptr<neure> p_neure_t;

    typedef std::unique_ptr<pack_pool<data_pack>> p_pool_t;

    typedef std::shared_ptr<data_action> p_data_action_t;

    typedef std::shared_ptr<polymer> p_polymer_t;
    typedef std::list<p_polymer_t> polymer_list_t;
    typedef std::shared_ptr<polymer_list_t> p_polymer_list_t;


    typedef std::function< bool(const p_object_t& )> walk_object_function;
    typedef std::function< void(const data_context &, const axon &, const data_pack & )> data_handler;
    typedef std::function< bool(const std::string&, const p_axon_t& )> walk_axon_function;

    template<class T> 
    struct deleter{
        inline void operator()(virtual_t* _p)  {
            delete static_cast<T*>(_p);  
        } 
    };

    class virtual_t{
    public:
        template<class T>
        static p_virtual_t make_ptr(std::shared_ptr<T> _p){
            return p_virtual_t(reinterpret_cast<virtual_t>(_p.get()), deleter<T>());
        }
    };

    class base_object{     
    public:
        p_object_t get_parent(){
            return _parent_object;
        }
    protected:
        p_object_t _parent_object{nullptr};
    };

    class object_container: public base_object{
    public:
        object_container(p_object_t _parent = nullptr):_parent_object(_parent){
        }
        p_object_t add_child(p_object_t _p){
            if(!_p){
                std::lock_guard<std::mutex> lk(_lock);
                _childs.push_back(_p);
                _p->_parent_object = this;
            }
            return _p;
        }

        object_container(const object_container& _other):base_object(_other._parent_object){
            //std::unique_lock<std::mutex> lock1(_lock, std::defer_lock);
            //std::unique_lock<std::mutex> lock2(_other._lock, std::defer_lock);
            //std::lock(lock1, lock2);
            //_childs = _other._childs;
            std::lock_guard lock(_other._lock);
            _childs = _other._childs;
        }

        object_container(object_container&& _other){
            _parent_object = _other._parent_object;
            std::lock_guard<std::mutex> lk(_lock);
            std::swap(_childs, _other._childs);
        }

        object_container(const object_list_t& _object_list):_childs(_object_list){
        }

        object_container(object_list_t&& _object_list){
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

    class data_format: public base_object{
    public:
    };
/*
    template<typename _struct_t = std::string>
    class data_format {

        static p_data_format_t get_unknow(){
            return p_data_format_t();
        }

        p_data_format_t&& to_data_format(){
            return std::move(p_data_format_t());
        }
    };*/

    typedef data_format<std::string> format_string;

    class data_value{
    public:
        bool as_bool();
        char as_char();
        unsigned char as_byte();
        short as_short();
        long as_long();
        long long as_longlong();
        std::string as_string();
    };

    class data_pack_builder {
    public:
        data_pack&& to_data_pack();
    };

    class data_pack
    {
    public:
        enum signal{before_changed,  after_changed};

        const data_value& operator[](std::string _path){
            //mapping path to index and find it
            return data_value();
        }

        const data_value& operator[](char* _path){
            if(!_path)
                return data_value();
            return (*this)[std::string(_path)];
        }

    protected:
    };


    class data_context : public data_pack{
    public:
    protected:
        p_spore_t _owner;
    };

    /*
    链，两端是axon，一个in， 一个out
    */
    class chain : base_object{
    public:
        chain():base_object(nullptr){
        }

        chain(p_axon_t _a1, p_axon_t _a2){
            if(_a1->_type == ~_a2->_type){
                //类型匹配判断
                //...
                //...
                if(_a1->_type == _meta_axon_type::IN_AXON)
                    _axon_in = _a1;
                else
                    _axon_out = _a1;
                if(_a2->_type == _meta_axon_type::IN_AXON)
                    _axon_in = _a2;
                else
                    _axon_out = _a2;
            }
        }
        bool is_valid(){
            return _axon_in && _axon_out;
        }

    protected:
        p_axon_t _axon_in{nullptr};
        p_axon_t _axon_out{nullptr};
    };

    /*
    polymer
    */
    class polymer : base_object{
    public:
        void polymer(const axon_list_t& _as, p_neure_t _n){
            _axon_list = _as;
            _neure = _n;
            check();
        }
        void is_avalid(){
            return _is_avalid;
        }
    protected:
        void check(){
            if(!_neure){
                return;
            }
            _is_avalid = true;
            for (p_axon_t &_a : _axon_list){
                if(_bindto == _meta_axon_type::UNKNOWN)
                    _bindto = _a->get_type();
                else
                    _is_avalid = _bindto == _a->get_type();
                if(!_is_avalid){
                    break;
                }
            }
            if(_is_avalid){
                _neure->walk_childs([&](const p_object_t& _p) -> bool{
                    if(!dynamic_pointer_cast<chain>(_p)){
                        _is_avalid = false;
                        return false;
                    }
                });
            }
            if(!_is_avalid){
                _bindto = _meta_axon_type::UNKNOWN;
            }
        }
    protected:
        _meta_axon_type _bindto{_meta_axon_type::UNKNOWN};
        bool _is_avalid{false};

        axon_list_t _axon_list;
        p_neure_t _neure{nullptr};
    }

    /*
    neure 一组以确定方式聚合的axon,chain或者子neure
    */
    enum neure_type{
        _unknow = 0,
        _addition = 1 << 16, 
        _multiplication = 1 << 17
    };
    class neure : public object_container{
    public:
        neure():object_container(nullptr){

        }
        neure(const neure& _other){
            _parent_object = _other._parent_object;
            _type = _other._type;
            std::unique_lock<std::mutex> lock1(_lock, std::defer_lock);
            std::unique_lock<std::mutex> lock2(_other._lock, std::defer_lock);
            std::lock(lock1, lock2);
            _childs = _other._childs;
        }
        neure(neure&& _other){
            _parent_object = _other._parent_object;
            _type = _other._type;
            std::lock_guard<std::mutex> lk(_lock);
            std::swap(_childs, _other._childs);
        }
        neure(neure_type _t, object_list_t &&_chs):_type(_t){
            std::swap(_childs, _chs);
        }
        neure(neure_type _t, const object_list_t &_chs):_type(_t){
            _childs = _chs;
        }

        neure_type get_type() const {
            return _type;
        }

    protected:
        neure_type _type{_unknow};
    }

    /*
    链接矩阵，把out neure链接到in neure，生成一组追加的链，然后通过这些链生成chain neure
    */
    class matrix
    {

    public:
        void matrix(){
        }
        void matrix(const matrix& _c):_p_spore_list(_c._p_spore_list),_p_in_neure(_c._p_in_neure),_p_out_neure(_c._p_out_neure){
        }
        void matrix(matrix&& _c):_p_in_neure(std::move(_c._p_in_neure)),_p_out_neure(std::move(_c._p_out_neure)){
            std::swap(_p_spore_list, _c._p_spore_list);
        }
        void matrix(p_spore_t  _p_s){
            if(_p_s){
                object_list_t _is;
                object_list_t _os;
                _p_s->walk_axon([&](const std::string& _name, const p_axon_t & _p)->bool{
                    if(_p){
                        if(_p->get_type() == IN_AXON){
                            _is.push_back(_p);
                        }else if(_p->get_type() == OUT_AXON){
                            _os.push_back(_p);
                        }
                    }
                    return true;
                });
                _p_in_neure = std::make_shared<neure>(neure_type::_multiplication, std::move(_is));
                _p_out_neure = std::make_shared<neure>(neure_type::_multiplication,std::move(_os));
            }
        }

        void matrix(p_axon_t _p_a){
            if(_p_a){
                if(_p_a->get_type() == IN_AXON)
                    _p_in_neure = std::make_shared<neure>(neure_type::_multiplication, {dynamic_pointer_cast<base_object>(_p_a)});
                else if(_p_a->get_type() == OUT_AXON)
                    _p_out_neure = std::make_shared<neure>(neure_type::_multiplication, {dynamic_pointer_cast<base_object>(_p_a)});
            }
        }

        void matrix(const std::vector<p_axon_t>& _p_as){
            object_list_t _is;
            object_list_t _os;
            for (auto &_a: _p_as){
                if(_a->get_type() == IN_AXON){
                    _is.push_back(_p);
                }else if(_a->get_type() == OUT_AXON){
                    _os.push_back(_p);
                }
            }
            _p_in_neure = std::make_shared<neure>(neure_type::_multiplication, std::move(_is));
            _p_out_neure = std::make_shared<neure>(neure_type::_multiplication,std::move(_os));
        }

        matrix&& operator*(const matrix &_other){
            return std::move(_union(*this, _other, neure_type::_multiplication));
        }

        matrix&& operator+(const matrix &_other){
            return std::move(_union(*this, _other, neure_type::_addition));
        }

        matrix&& operator>>(const matrix &_other){
            //这里生成链，再把链封装成neure，追加的out axon里

            return matrix();
        }

    protected:
        static matrix&& _union(const matrix &_m1, const matrix &_m2, neure_type _type){
            matrix _m;
            if(_m2._p_spore_list || _m2._p_spore_list)
                _m._p_spore_list = std::make_shared<spore_list_t>();
            else{
                if(_m2._p_spore_list)
                    _m._p_spore_list->assign(_m2._p_spore_list->begin(), _m2._p_spore_list->end());
                if(_m1._p_spore_list)
                    _m._p_spore_list->assign(_m1._p_spore_list->begin(), _m1._p_spore_list->end());
            }
            if(_m2._p_polymer_list || _m2._p_polymer_list)
                _m._p_polymer_list = std::make_shared<polymer_list_t>();
            else{
                if(_m2._p_polymer_list)
                    _m._p_polymer_list->assign(_m2._p_polymer_list->begin(), _m2._p_polymer_list->end());
                if(_m1._p_polymer_list)
                    _m._p_polymer_list->assign(_m1._p_polymer_list->begin(), _m1._p_polymer_list->end());
            }
            _m._p_in_neure = std::make_shared<neure>(_type, {dynamic_pointer_cast<base_object>(_m1._p_in_neure), dynamic_pointer_cast<base_object>(_m2._p_in_neure)});
            _m._p_out_neure = std::make_shared<neure>(_type, {dynamic_pointer_cast<base_object>(_m1._p_out_neure), dynamic_pointer_cast<base_object>(_m2._p_out_neure)});
            return std::move(_m);
        }

        static matrix&& _connect(const matrix &_from, const matrix &_to){
            //_m2 必须包含输入
            if(!_to._p_in_neure || !_to._p_in_neure->has_childs()){
                //MSG...
                return std::move(matrix());
            }
            if(!_from._p_out_neure || !_from._p_out_neure->has_childs())
                return _union(_from, _to, neure_type::_multiplication); 
            matrix _m;
            _connect(_m, _from._p_out_neure, _to._p_in_neure);
            if(!_m._p_spore_list && (_from._p_spore_list || _to._p_spore_list))
                _m._p_spore_list = std::make_shared<spore_list_t>();
            if(_from._p_spore_list)
                _m._p_spore_list->assign(_from._p_spore_list->begin(), _from._p_spore_list->end());
            if(_to._p_spore_list)
                _m._p_spore_list->assign(_to._p_spore_list->begin(), _to._p_spore_list->end());
            if(!_m._p_polymer_list && (_from._p_polymer_list || _to._p_polymer_list))
                _m._p_polymer_list = std::make_shared<polymer_list_t>();
            if(_from._p_polymer_list)
                _m._p_polymer_list->assign(_from._p_polymer_list->begin(), _from._p_polymer_list->end());
            if(_to._p_polymer_list)
                _m._p_polymer_list->assign(_to._p_polymer_list->begin(), _to._p_polymer_list->end());
            return std::move(_m);
        }

        static void _connect(matrix &_matrix, p_neure_t _from_neure, p_neure_t _to_neure){
            object_list_t _from_objs;
            _from_neure->walk_childs([&](const p_object_t& _obj) -> bool{
                p_neure_t _neure = dynamic_pointer_cast<neure>(_obj);
                if(_neure){
                    p_spore_t _spore = std::make_shared<spore_odd>();
                    if(!_matrix._p_spore_list)
                        _matrix._p_spore_list = std::make_shared<spore_list_t>();
                    _matrix._p_spore_list->push_back(_spore);
                    matrix _m(_spore);
                    _connect(_matrix, _neure, _m._p_in_neure);
                    _from_objs.push_back(_m._p_out_neure);
                }else if(dynamic_pointer_cast<axon>(_obj)){
                    _from_objs.push_back(_obj);
                }
            });

            object_list_t _to_objs;
            _to_neure->walk_childs([&](const p_object_t& _obj) -> bool{
                p_neure_t _neure = dynamic_pointer_cast<neure>(_obj);
                if(_neure){
                    p_spore_t _spore = std::make_shared<spore_odd>();
                    if(!_matrix._p_spore_list)
                        _matrix._p_spore_list = std::make_shared<spore_list_t>();
                    _matrix._p_spore_list->push_back(_spore);
                    matrix _m(_spore);
                    _connect(_matrix, _m._p_out_neure, _neure);
                    _to_objs.push_back(_m._p_in_neure);
                }else if(dynamic_pointer_cast<axon>(_obj)){
                    _to_objs.push_back(_obj);
                }
            });
            if(_from_objs.size() > 1 && _to_objs.size() > 1){
                p_spore_t _spore = std::make_shared<spore_odd>();
                _matrix._p_spore_list->push_back(_spore);
                matrix _m(_spore);
                _connect(_matrix, _m._p_out_neure, std::make_shared<neure>(_to_objs));
                _to_objs.clear();
                _to_objs.push_back(_m._p_in_neure);
            }else
            {

            }
        }

    protected:
        p_neure_t _p_in_neure{nullptr};
        p_neure_t _p_out_neure{nullptr};
        p_spore_list_t _p_spore_list{nullptr};
        p_polymer_list_t _p_polymer_list{nullptr};
    };

    class axon_builder;
    class axon{
    public:
        enum  signal{data_in};

        data_pack_builder&& get_data_pack_builder(){
            std::move(data_pack_builder());
        }

        void push(data_pack&& _data_pack){

        }
        void push(const data_pack& _data_pack){

        }

        _meta_axon_type get_type() const{
            std::lock_guard<std::mutex> lk(_lock_builder);
            return _type;
        }

        p_data_format_t get_data_format() const{
            std::lock_guard<std::mutex> lk(_lock_builder);
            return _data_format;
        }

        p_actuator_t get_actuator() const{
            std::lock_guard<std::mutex> lk(_lock_builder);
            return _actuator;
        }

        matrix&& connect(p_axon_t _other){
            if(_type != ~_other->_type)
            {
                //MSG... ERROR
                return std::move(matrix());
            }
            return std::move(matrix({std::make_shared<chain>(shared_ptr<axon>(this) , _other)}));
        }

        matrix&& operator<<(p_axon_t _other){
            return std::move(matrix(shared_ptr<axon>(this)) << matrix(_other));
        }

        matrix&& operator>>(p_axon_t _other){
            return std::move(matrix(shared_ptr<axon>(this)) >> matrix(_other));
        }

    protected:
        friend class cluster;
        friend class spore;
        friend class axon_builder;

        _meta_axon_type _type{UNKNOWN};
        p_data_format_t _data_format{nullptr};
        p_actuator_t _actuator{nullptr};
    };

    struct data_action{
        data_action(const data_trigger _t, const data_handler &_h):_trigger(_t), _handler(_h){
        }
        data_trigger _trigger;
        data_handler _handler;
    };

    class axon_builder : public axon{
    public:
        void reset(){
            std::lock_guard<std::mutex> lk(_lock_builder);
            _action_list.clear();
            _type = _meta_axon_type::UNKNOWN;
            _data_format = nullptr;
            _actuator = nullptr;
        }

        void set_type(_meta_axon_type _t){
            std::lock_guard<std::mutex> lk(_lock_builder);
            _type = _t;
        }

        void set_data_format(p_data_format_t _f){
            std::lock_guard<std::mutex> lk(_lock_builder);
            _data_format = _f;
        }

        void set_actuator(const p_actuator_t& _act){
            std::lock_guard<std::mutex> lk(_lock_builder);
            _actuator = _act;
        }

        void add_action(const data_trigger& _trigger, const data_handler& _action){
            std::lock_guard<std::mutex> lk(_lock_builder);
            _action_list.push_back(std::make_shared<data_action>(_trigger, _actuator));
        }

        void add_action(axon::signal _signal, const data_handler& _action){
            add_action(data_trigger(this, _signal), std::move(_action));
        }

        p_axon_t to_axon() const{
            std::lock_guard<std::mutex> lk(_lock_builder);
            p_axon_t _a = std::make_shared<axon>();
            _a->_type = _type;
            _a->_data_format = _data_format;
            _a->_actuator = _actuator;
            return _a;
        }

    protected:
        mutable std::mutex _lock_builder;
        std::list<p_data_action_t> _action_list;
    };


    class neure{
    public:
    };


    /*
    //typedef std::function< bool(const p_axon_t & )> walk_axon_function;
    class spore{
    public:
    spore(){
    }
    //void walk_axon(_meta_axon_type _type, const walk_axon_function &_func){
    //    if(!_func) return;
    //    for (std::vector<p_axon_t>::const_iterator ite = _axon_array.begin(); ite != _axon_array.end(); ite ++){
    //        if(!(_func)(*ite))
    //            return;
    //    }
    //}
    unsigned int get_actuator_alloc_count(){
    return 0;
    }
    protected:
    friend class cluster;
    friend class spore_builder;
    cluster *_p_cluster{nullptr};
    std::vector<p_axon_t> _axon_array;
    //p_axon_t _axon_array{nullptr};//=shared_ptr<axon>(new axon[size], default_delete<axon[]>()

    };
    */

    class spore_builder;
    class spore{
    public:
        spore(){

        }

        p_spore_t clone(){
            return p_spore_t();
        }

        p_spore_t newborn(){
            return p_spore_t();
        }

        void walk_axon(const walk_axon_function &_func){
            if(!_func) return;
            for (auto ite = _axon_array.begin(); ite != _axon_array.end(); ite ++){
                if(!(_func)(ite->first, ite->second))
                    return;
            }
        }
    protected:
        std::map<std::string, p_data_format_t> _context_array;
        std::map<std::string, p_axon_t> _axon_array;
        std::list<p_data_action_t> _action_list;
    };

    class spore_builder: public spore{
    public:
        p_data_format_t add_data_format(std::string _format_name,  const p_data_format_t& _p_data_format_t){
            if(!_p_data_format_t)
                return nullptr;
            p_data_format_t _p{nullptr};
            std::lock_guard<std::mutex> lk(_lock_builder);
            _context_array[_format_name] = _p = std::make_shared<data_format>(*_p_data_format_t->get());
            return _p;
        }
        p_axon_t add_axon(std::string _axon_name, const axon_builder& _axon_builder){
            p_axon_t _p = _axon_builder.to_axon();
            std::lock_guard<std::mutex> lk(_lock_builder);
            _action_list.merge(_axon_builder._action_list);
            _axon_array[_axon_name] = _p;
            return _p;
        }

        void add_trigger(const data_trigger& _trigger, const data_handler& _action){
            std::lock_guard<std::mutex> lk(_lock_builder);
            _action_list.push_back(std::make_shared<data_action>(_trigger, _actuator));
        }
    protected:
        std::mutex _lock_builder;
    };


    class spore_odd : public spore{
        p_axon_t _out_axon;
        p_axon_t _in_axon;
    public:
        spore_odd(){
            axon_builder _builder;
            _builder.set_type(_meta_axon_type::OUT);
            _builder.set_data_format(data_format::get_unknow());
            _out_axon = add("_out", _builder);
            _builder.reset();
            _builder.set_type(_meta_axon_type::IN_AXON);
            _builder.set_data_format(data_format::get_unknow());
            _builder.add_action(axon::signal::data_in,
                [&](const data_context & _context, const axon & _axon, const data_pack & _in_pack){
                    _out_axon->push(&_in_pack);
            });
            _in_axon = add("_in", _builder);
        }
        p_axon_t get_in(){
            return _in_axon;
        }
        p_axon_t get_out(){
            return _out_axon;
        }
    };



    class data_trigger{
    public:
        /*
        1. handel, 用于描述触发这个trigger的来源
        2. signal, 描述这个源的什么信号会导致触发
        */
        /*for data_pack*/
        data_trigger(const data_pack& _data_pack, data_format_path&& _path, data_pack::signal _signal){
        }
        /*for axon*/
        data_trigger(const p_axon_t& _axon, axon::signal _signal){

        }

    };

    class cluster{
    public:
        enum ability_t{
            anonymous = 1 << 0,
            visible =          1 << 1, 
            query =           1 << 2, 
            transfer =        1 << 3, 
            performer =    1 << 4,
            dispatcher =    1 << 5,
            master =         1 << 6,
            sages =            visible | query | transfer | performer | dispatcher | master,
        };
        enum status_t{
            original,
            initialize,
            running,
            releasing,
            finish,
            fail
        };
        cluster(const cluster&) = delete;
        cluster(const cluster&&) = delete;
        cluster& operator=(const cluster&) = delete;
        cluster(unsigned int actuator_max_count = std::thread::hardware_concurrency() + 1,
            ability_t ability = ability_t::anonymous):
        _actuator_max_count(actuator_max_count),_ability(ability){
        }

        bool run(const std::vector<spore> &_spores){
            std::lock_guard<std::mutex> lk(_lock_operate);
            if(_status != status_t::original)
                return false;
            _status = status_t::initialize;

            //reg all spore....

            init_neure_builder();
            //scanning
            _actuator_alloc_count = 0;
            std::for_each(_spores.begin(), _spores.end(), [&](spore &_spore){
                _spore._p_cluster = this;
                _actuator_alloc_count += _spore.get_actuator_alloc_count();

                //distribute  the neure builder

            });
            init_pool();
            if(_actuator_max_count - _actuator_alloc_count < 1){
                _status = status_t::fail;
                return false;
            }
            return true;
        }

        p_spore_t&& reg(std::string _name, p_spore_t _spore){
            if(!_spore)
                return nullptr;

            //reg spore

            return std::move(p_spore_t(_spore));
        } 

        p_data_format_t&& reg(std::string _name, p_data_format_t _format){
            if(!_spore)
                return nullptr;

            //reg data format

            return std::move(p_data_format_t(_format));
        }

    protected:
        void pack_action(data_pack& _pack){
            //push to pack to list
        }

        data_pack&& fill_empty_pack(){
            std::move(data_pack());
        }

        void init_neure_builder(){
            std::call_once(_init_neure_builder_flag, [&](){
                _p_neure_builder = std::make_shared<neure>();
            });

        }

        void init_pool(){
            std::call_once(_init_pool_flag, [&](){
                unsigned int base_thread_count = _actuator_max_count - _actuator_alloc_count;
                if(base_thread_count > 0){
                    _p_pool = std::make_unique<pack_pool<data_pack>>(
                        std::bind(&cluster::pack_action, this),
                        std::bind(&cluster::fill_empty_pack, this),
                        base_thread_count
                        );
                }
            });
        }
    protected:
        const unsigned int _actuator_max_count{0};
        const ability_t _ability{ability_t::anonymous};
        unsigned int _actuator_alloc_count{0};

        p_pool_t _p_pool;
        p_neure_t _p_neure_builder;
        std::once_flag _init_pool_flag;
        std::once_flag _init_neure_builder_flag;
        std::mutex _lock_operate;
        status_t _status{status_t::original};
    };

}





#endif // ELEMENT_H_INCLUDED